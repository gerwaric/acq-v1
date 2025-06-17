// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QByteArray>
#include <QByteArrayView>
#include <QDateTime>
#include <QString>
#include <QStringView>

namespace rfc2822 {

    QDateTime parse(QStringView s);

    inline QString fix(QStringView string)
    {
        return rfc2822::parse(string).toString(Qt::RFC2822Date);
    }

    inline QByteArray fix(QByteArrayView bv)
    {
        return rfc2822::fix(QString::fromUtf8(bv)).toUtf8();
    }

    inline QDateTime parse(QByteArrayView utf)
    {
        return rfc2822::parse(QString::fromUtf8(utf));
    }

} // namespace rfc2822
