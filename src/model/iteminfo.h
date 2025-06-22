// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"

#include <QString>

#include <type_traits>

class ItemInfo;

namespace {

    struct Column
    {
        const char *header;
        QVariant (*getter)(ItemInfo const &);
    };

    template<auto MemberPtr>
    static QVariant get(ItemInfo const &i)
    {
        // 1) figure out the *actual* field type, e.g. poe::DisplayMode
        using T = std::remove_cv_t<std::remove_reference_t<decltype(i.*MemberPtr)> >;

        if constexpr (std::is_enum_v<T>) {
            if constexpr (QMetaTypeId<T>::Defined) {
                // 2) if the enum is registered with Qt’s meta-type system, use fromValue()
                return QVariant::fromValue(i.*MemberPtr);
            } else {
                // 3) for all other enums cast to the underlying integer.
                return QVariant(std::to_underlying(i.*MemberPtr));
            }
        } else {
            // 4) all your other fields (QString, int, float, bool…) just go straight into QVariant
            return QVariant(i.*MemberPtr);
        }
    }

} // namespace

struct ItemInfo
{
    ItemInfo(const poe::Item& item);

    QString id;
    QString name;
    QString typeLine;
    QString prettyName;
    QString baseType;
    QString itemCategory;
    poe::FrameType itemRarity;

    float damage{0.0};
    float criticalChance{0.0};
    float physicalDps{0.0};
    float attacksPerSecond{0.0};
    float totalDps{0.0};
    float elementalDps{0.0};
    float chaosDps{0.0}; // custom extension

    int armour{0};
    int energyShield{0};
    int block{0};
    int evasionRating{0};
    int ward{0};
    int baseBercentile{0};

    int redSockets{0};
    int greenSockets{0};
    int blueSockets{0};
    int whiteSockets{0};
    int abyssSockets{0}; // custom extension
    int totalSockets{0};

    struct SocketGroup {
        unsigned red;
        unsigned green;
        unsigned blue;
        unsigned white;
        unsigned abyss;
    };
    std::vector<SocketGroup> socketGroups;

    int requiredLevel{0};
    int requiredStrength{0};
    int requiredDexterity{0};
    int requiredIntelligence{0};
    QString requiredClass;

    int requiredLockingLevel{0};
    int requiredBruteForceLevel{0};
    int requiredPerceptionLevel{0};
    int requiredDemolutionLevel{0};
    int requiredCounterThaumaturgyLevel{0};
    int requiredTrapDisarmamentLevel{0};
    int requiredAgilityLevel{0};
    int requiredDeceptionLevel{0};
    int requiredEngineeringLevel{0};

    int quality{0};
    int itemLevel{0};
    int gemLevel{0};
    int gemExperience{0};

    bool transiguredGem{false};
    bool vaalGem{false};
    QString corpseType;
    bool crucible{false};
    bool fractured{false};
    bool synthesised{false};
    bool searingExarch{false};
    bool eaterOfWorlds{false};
    bool identified{false};
    bool corrupted{false};
    bool mirrored{false};
    bool split{false};
    bool crafted{false};
    bool veiled{false};
    bool forseeing{false};
    int talismanTier{0};
    int storedExperience{0};
    int stackSize{0};
    QString alternateArt;
    QString foilVariation;
    int scourgeTier{0};

    // clang-format off
    static constexpr Column Columns[] = {
        //{ "Id",          &get<&ItemInfo::id>                   },
        { "Name",        &get<&ItemInfo::prettyName>           },
        { "Rarity",      &get<&ItemInfo::itemRarity>           },
        { "Base Type",   &get<&ItemInfo::baseType>             },
        { "Armour",      &get<&ItemInfo::armour>               },
        { "Evasion",     &get<&ItemInfo::evasionRating>        },
        { "ES",          &get<&ItemInfo::energyShield>         },
        { "Req. Level",  &get<&ItemInfo::requiredLevel>        },
        { "Req. Str",    &get<&ItemInfo::requiredStrength>     },
        { "Req. Dex",    &get<&ItemInfo::requiredDexterity>    },
        { "Req. Int",    &get<&ItemInfo::requiredIntelligence> },
    };
    // clang-format on

    static constexpr int ColumnCount = sizeof(Columns) / sizeof(*Columns);

private:
    // Use static helpers because the member names are not prefixed with m_*,
    // so it would be very easy to create local variables that accidentally
    // masked member fields. With this approach, every member access will
    // be obvious because it will start look like i.*
    static void loadSockets(const poe::Item &item, ItemInfo &i);
    static void loadProperties(const poe::Item &item, ItemInfo &i);
    static void loadRequirements(const poe::Item &item, ItemInfo &i);

    static float average(const QString &ranged_value, bool *ok);
};
