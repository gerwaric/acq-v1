// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe/types/item.h"

#include "util/glaze_qt.h"

#include <QString>

#include <type_traits>

class ItemInfo;

namespace {

    struct Column
    {
        const char *name;
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
    ItemInfo(const poe::Item &item);

    QString id;
    QString name;
    QString typeLine;
    QString prettyName;
    QString baseType;
    QString itemCategory;
    QString icon;

    poe::FrameType frameType;

    unsigned w{0};
    unsigned h{0};

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

    struct SocketGroup
    {
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

    // Influences
    bool shaper{false};
    bool elder{false};
    bool crusader{false};
    bool redeemer{false};
    bool hunter{false};
    bool warlord{false};

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

    static constexpr Column Columns[] = {
        {"Name", &get<&ItemInfo::prettyName>},
        {"Armour", &get<&ItemInfo::armour>},
        {"Evasion", &get<&ItemInfo::evasionRating>},
        {"Energy Shield", &get<&ItemInfo::energyShield>},
        /*
        {"Min Level", &get<&ItemInfo::requiredLevel>},
        {"Min Str", &get<&ItemInfo::requiredStrength>},
        {"Min Dex", &get<&ItemInfo::requiredDexterity>},
        {"Min Int", &get<&ItemInfo::requiredIntelligence>},
        */
    };

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

// clang-format off
template <>
struct glz::meta<ItemInfo> {
    static constexpr auto value = glz::object(

        &ItemInfo::id, "id",
        &ItemInfo::name, "name",
        &ItemInfo::typeLine, "typeLine",
        &ItemInfo::prettyName, "prettyName",
        &ItemInfo::baseType, "baseType",
        &ItemInfo::itemCategory, "itemCategory",
        &ItemInfo::frameType, "frameType",
        
        &ItemInfo::damage, "damage",
        &ItemInfo::criticalChance, "criticalChance",
        &ItemInfo::physicalDps, "physicalDps",
        &ItemInfo::attacksPerSecond, "attacksPerSecond",
        &ItemInfo::totalDps, "totalDps",
        &ItemInfo::elementalDps, "elementalDps",
        &ItemInfo::chaosDps, "chaosDps",
        
        &ItemInfo::armour, "armour",
        &ItemInfo::energyShield, "energyShield",
        &ItemInfo::block, "block",
        &ItemInfo::evasionRating, "evasionRating",
        &ItemInfo::ward, "ward",
        &ItemInfo::baseBercentile, "baseBercentile",
        
        &ItemInfo::redSockets, "redSockets",
        &ItemInfo::greenSockets, "greenSockets",
        &ItemInfo::blueSockets, "blueSockets",
        &ItemInfo::whiteSockets, "whiteSockets",
        &ItemInfo::abyssSockets, "abyssSockets",
        &ItemInfo::totalSockets, "totalSockets",
        
        &ItemInfo::requiredLevel, "requiredLevel",
        &ItemInfo::requiredStrength, "requiredStrength",
        &ItemInfo::requiredDexterity, "requiredDexterity",
        &ItemInfo::requiredIntelligence, "requiredIntelligence",
        &ItemInfo::requiredClass, "requiredClass",
        
        &ItemInfo::requiredLockingLevel, "requiredLockingLevel",
        &ItemInfo::requiredBruteForceLevel, "requiredBruteForceLevel",
        &ItemInfo::requiredPerceptionLevel, "requiredPerceptionLevel",
        &ItemInfo::requiredDemolutionLevel, "requiredDemolutionLevel",
        &ItemInfo::requiredCounterThaumaturgyLevel, "requiredCounterThaumaturgyLevel",
        &ItemInfo::requiredTrapDisarmamentLevel, "requiredTrapDisarmamentLevel",
        &ItemInfo::requiredAgilityLevel, "requiredAgilityLevel",
        &ItemInfo::requiredDeceptionLevel, "requiredDeceptionLevel",
        &ItemInfo::requiredEngineeringLevel, "requiredEngineeringLevel",
        
        &ItemInfo::quality, "quality",
        &ItemInfo::itemLevel, "itemLevel",
        &ItemInfo::gemLevel, "gemLevel",
        &ItemInfo::gemExperience, "gemExperience",
        
        &ItemInfo::transiguredGem, "transiguredGem",
        &ItemInfo::vaalGem, "vaalGem",
        &ItemInfo::corpseType, "corpseType",
        &ItemInfo::crucible, "crucible",
        &ItemInfo::fractured, "fractured",
        &ItemInfo::synthesised, "synthesised",
        &ItemInfo::searingExarch, "searingExarch",
        &ItemInfo::eaterOfWorlds, "eaterOfWorlds",
        &ItemInfo::identified, "identified",
        &ItemInfo::corrupted, "corrupted",
        &ItemInfo::mirrored, "mirrored",
        &ItemInfo::split, "split",
        &ItemInfo::crafted, "crafted",
        &ItemInfo::veiled, "veiled",
        &ItemInfo::forseeing, "forseeing",
        &ItemInfo::talismanTier, "talismanTier",
        &ItemInfo::storedExperience, "storedExperience",
        &ItemInfo::stackSize, "stackSize",
        &ItemInfo::alternateArt, "alternateArt",
        &ItemInfo::foilVariation, "foilVariation",
        &ItemInfo::scourgeTier, "scourgeTier"
    );
};

// clang-format on    
