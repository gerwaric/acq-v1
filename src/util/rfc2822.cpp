// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "rfc2822.h"

#include <QTimeZone>

// Adapted from https://code.qt.io/cgit/qt/qtbase.git/tree/src/corelib/time/qdatetime.cpp?h=6.9.0

namespace {

    struct ParsedRfcDateTime
    {
        QDate date;
        QTime time;
        int utcOffset = 0;
    };

    static int shortDayFromName(QStringView name)
    {
        const char16_t shortDayNames[] = u"MonTueWedThuFriSatSun";
        for (int i = 0; i < 7; i++) {
            if (name == QStringView(shortDayNames + 3 * i, 3))
                return i + 1;
        }
        return 0;
    }

    static const char qt_shortMonthNames[][4]
        = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    static int fromShortMonthName(QStringView monthName)
    {
        for (unsigned int i = 0; i < sizeof(qt_shortMonthNames) / sizeof(qt_shortMonthNames[0]);
             ++i) {
            if (monthName == QLatin1StringView(qt_shortMonthNames[i], 3))
                return i + 1;
        }
        return -1;
    }

    // ADDED
    static const char qt_obsZones[][4] = {"GMT", "EST", "EDT", "CST", "CDT", "PST", "PDT"};
    static const int qt_obsOffsets[] = {0, -5, -4, -6, -5, -8, -7};
    static int offsetFromObsZone(QStringView obsZone, bool &ok)
    {
        for (unsigned int i = 0; i < sizeof(qt_obsZones) / sizeof(qt_obsZones[0]); ++i) {
            if (obsZone == QLatin1StringView(qt_obsZones[i], 4)) {
                ok = true;
                return qt_obsOffsets[i];
            }
        }
        ok = false;
        return 0;
    }

    static ParsedRfcDateTime rfcDateImpl(QStringView s)
    {
        // Matches "[ddd,] dd MMM yyyy[ hh:mm[:ss]] [±hhmm]" - correct RFC 822, 2822, 5322 format -
        // or           "ddd MMM dd[ hh:mm:ss] yyyy [±hhmm]" - permissive RFC 850, 1036 (read only)
        ParsedRfcDateTime result;

        QVarLengthArray<QStringView, 6> words;

        auto tokens = s.tokenize(u' ', Qt::SkipEmptyParts);
        auto it = tokens.begin();
        for (int i = 0; i < 6 && it != tokens.end(); ++i, ++it)
            words.emplace_back(*it);

        if (words.size() < 3 || it != tokens.end())
            return result;
        const QChar colon(u':');
        bool ok = true;
        QDate date;

        const auto isShortName = [](QStringView name) {
            return (name.size() == 3 && name[0].isUpper() && name[1].isLower() && name[2].isLower());
        };

        /* Reject entirely (return) if the string is malformed; however, if the date
         * is merely invalid, (break, so as to) go on to parsing of the time.
         */
        int yearIndex;
        do { // "loop" so that we can use break on merely invalid, but "right shape" date.
            QStringView dayName;
            bool rfcX22 = true;
            const QStringView maybeDayName = words.front();
            if (maybeDayName.endsWith(u',')) {
                dayName = maybeDayName.chopped(1);
                words.erase(words.begin());
            } else if (!maybeDayName.front().isDigit()) {
                dayName = maybeDayName;
                words.erase(words.begin());
                rfcX22 = false;
            } // else: dayName is not specified (so we can only be RFC *22)
            if (words.size() < 3 || words.size() > 5)
                return result;

            // Don't break before setting yearIndex.
            int dayIndex, monthIndex;
            if (rfcX22) {
                // dd MMM yyyy [hh:mm[:ss]] [±hhmm]
                dayIndex = 0;
                monthIndex = 1;
                yearIndex = 2;
            } else {
                // MMM dd[ hh:mm:ss] yyyy [±hhmm]
                dayIndex = 1;
                monthIndex = 0;
                yearIndex = words.size() > 3 && words.at(2).contains(colon) ? 3 : 2;
            }

            int dayOfWeek = 0;
            if (!dayName.isEmpty()) {
                if (!isShortName(dayName))
                    return result;
                dayOfWeek = shortDayFromName(dayName);
                if (!dayOfWeek)
                    break;
            }

            const int day = words.at(dayIndex).toInt(&ok);
            if (!ok)
                return result;
            const int year = words.at(yearIndex).toInt(&ok);
            if (!ok)
                return result;
            const QStringView monthName = words.at(monthIndex);
            if (!isShortName(monthName))
                return result;
            int month = fromShortMonthName(monthName);
            if (month < 0)
                break;

            date = QDate(year, month, day);
            if (dayOfWeek && date.dayOfWeek() != dayOfWeek)
                date = QDate();
        } while (false);
        words.remove(yearIndex);
        words.remove(0, 2); // month and day-of-month, in some order

        // Time: [hh:mm[:ss]]
        QTime time;
        if (words.size() && words.at(0).contains(colon)) {
            const QStringView when = words.front();
            words.erase(words.begin());
            if (when.size() < 5 || when[2] != colon
                || (when.size() == 8 ? when[5] != colon : when.size() > 5)) {
                return result;
            }
            const int hour = when.first(2).toInt(&ok);
            if (!ok)
                return result;
            const int minute = when.sliced(3, 2).toInt(&ok);
            if (!ok)
                return result;
            const auto secs = when.size() == 8 ? when.last(2).toInt(&ok) : 0;
            if (!ok)
                return result;
            time = QTime(hour, minute, secs);
        }

        // Offset = [±hh[mm] | obz-zone]
        int offset = 0;
        if (words.size()) {
            const QStringView zone = words.front();
            words.erase(words.begin());
            if ((zone[0] == u'-' || (zone[0] == u'+'))
                && ((zone.size() == 3) || (zone.size() == 5))) {
                // Offset is ±hh[mm]
                const int hour = zone.sliced(1, 2).toInt(&ok);
                if (!ok)
                    return result;
                const auto minute = zone.size() == 5 ? zone.last(2).toInt(&ok) : 0;
                if (!ok)
                    return result;
                offset = (hour * 60 + minute) * 60;
                if (zone[0] == u'-')
                    offset = -offset;
            } else {
                // Obffset is obs-zone
                if (zone.size() == 2) {
                    // obz-zone = UT (-0000)
                    if (zone != QLatin1StringView("UT", 3))
                        return result;
                } else if (zone.size() == 3) {
                    // obz-zone = GMT / EST / EDT / CST / CDT / PST / PDT
                    offset = offsetFromObsZone(zone, ok);
                    if (!ok)
                        return result;
                } else if (zone.size() == 1) {
                    // obz-zone = 1ALPHA (RFC2822 specifies A-I and K-Z are valid but should be interpretted as "-0000")
                    if (zone[0] == u'J')
                        return result;
                    if (((zone[0] < u'A') || (zone[0] > u'Z'))
                        && ((zone[0] < u'a') && (zone[0] > u'z')))
                        return result;
                } else {
                    // Invalid zone
                    return result;
                }
            }
        }

        result.date = date;
        result.time = time;
        result.utcOffset = offset;
        return result;
    }

} // namespace

QDateTime rfc2822::parse(QStringView string)
{
    if (string.isEmpty())
        return QDateTime();

    const ParsedRfcDateTime rfc = rfcDateImpl(string);

    if (!rfc.date.isValid() || !rfc.time.isValid())
        return QDateTime();

    QDateTime dateTime(rfc.date, rfc.time, QTimeZone::UTC);
    dateTime.setTimeZone(QTimeZone::fromSecondsAheadOfUtc(rfc.utcOffset));
    return dateTime;
}
