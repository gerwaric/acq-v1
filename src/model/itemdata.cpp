// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/itemdata.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG);

const std::vector<ItemData::Column> ItemData::Columns = ItemData::createColumnsInfo();
const int ItemData::ColumnCount = ItemData::Columns.size();

ItemData::ItemData(const poe::Item &item)
{
    id = item.id.value_or("");
    name = item.name;
    typeLine = item.typeLine;
    baseType = item.baseType;
    frameType = item.frameType;

    if (name.isEmpty()) {
        prettyName = typeLine;
    } else {
        prettyName = name + " " + typeLine;
    }

    const auto propertySections = ItemData::formatProperties(item);

    if (propertySections.size() > 0) {
        propertiesText1 = propertySections.front();
    }
    if (propertySections.size() > 1) {
        propertiesText2 = propertySections.back();
    }
    if (propertySections.size() > 2) {
        spdlog::error("Encountered a item with {} property sections", propertySections.size());
    }

    requirementsText = ItemData::formatRequirements(item);

    implicitMods = ItemData::getMods(item.implicitMods);
    enchantMods = ItemData::getMods(item.enchantMods);
    fracturedMods = ItemData::getMods(item.fracturedMods);
    explicitMods = ItemData::getMods(item.explicitMods);
    craftedMods = ItemData::getMods(item.craftedMods);

    w = item.w;
    h = item.h;
    icon = item.icon;

    loadSockets(item, *this);
    loadProperties(item, *this);
    loadRequirements(item, *this);

    if (item.influences) {
        const auto &influences = item.influences.value();
        shaper = influences.shaper.value_or(false);
        elder = influences.elder.value_or(false);
        crusader = influences.crusader.value_or(false);
        redeemer = influences.redeemer.value_or(false);
        hunter = influences.hunter.value_or(false);
        warlord = influences.warlord.value_or(false);
    }

    //bool transigured_gem;
    if (item.hybrid) {
        vaalGem = item.hybrid->isVaalGem.value_or(false);
    } else {
        vaalGem = false;
    }
    //QString corpse_type;
    crucible = item.crucibleMods.has_value();
    fractured = item.fractured.value_or(false);
    synthesised = item.synthesised.value_or(false);
    searingExarch = item.searing.value_or(false);
    eaterOfWorlds = item.tangled.value_or(false);
    identified = item.identified;
    corrupted = item.corrupted.value_or(false);
    //bool mirrored;
    split = item.split.value_or(false);
    crafted = item.craftedMods.has_value();
    veiled = item.veiled.value_or(false);
    forseeing = item.foreseeing.value_or(false);
    talismanTier = item.talismanTier.value_or(0);
    //int stored_experience;
    stackSize = item.stackSize.value_or(0);
    //QString alternate_art;
    //QString foil_variation;
    //int scourge_tier;

}

ItemData::WeaponData &ItemData::weapon()
{
    if (!weaponData) {
        weaponData = WeaponData{};
    }
    return *weaponData;
}

ItemData::ArmourData &ItemData::armour()
{
    if (!armourData) {
        armourData = ArmourData{};
    }
    return *armourData;
}

ItemData::SocketData &ItemData::sockets()
{
    if (!socketData) {
        socketData = SocketData{};
    }
    return socketData.value();
};

ItemData::HeistData &ItemData::heist()
{
    if (!heistData) {
        heistData = HeistData{};
    }
    return *heistData;
};

void ItemData::loadSockets(const poe::Item &item, ItemData &i)
{
    if (!item.sockets) {
        return;
    }

    // Extract sockets from the item.
    SocketData sockets{};
    for (const auto& socket : item.sockets.value()) {
        const unsigned int group = socket.group;
        while (group >= sockets.socketGroups.size()) {
            sockets.socketGroups.push_back(SocketGroup{});
        }
        const QString& colour = socket.sColour.value_or("");
        // clang-format off
        if      (colour == "R")  { ++sockets.redSockets;   ++sockets.socketGroups[group].red;   }
        else if (colour == "G")  { ++sockets.greenSockets; ++sockets.socketGroups[group].green; }
        else if (colour == "B")  { ++sockets.blueSockets;  ++sockets.socketGroups[group].blue;  }
        else if (colour == "W")  { ++sockets.whiteSockets; ++sockets.socketGroups[group].white; }
        else if (colour == "A")  { ++sockets.abyssSockets; ++sockets.socketGroups[group].abyss; }
        else if (colour == "DV") { /* ignore resonator sockets*/ }
        else {
            spdlog::error("Unhandled item socket in {} {}: {}", item.name, item.typeLine, colour);
        }
        // clang-format on
        ++sockets.totalSockets;
    }

    // Save sockets into the output structure.
    i.socketData = std::move(sockets);
}

void ItemData::loadProperties(const poe::Item &item, ItemData &i)
{
    if (!item.properties) {
        return;
    }

    float avg_phys_hit = 0;
    float avg_ele_hit = 0;
    float avg_chaos_hit = 0;

    for (const auto &property : item.properties.value()) {
        // This function only parses the typed properties.
        if (!property.type) {
            continue;
        }

        const poe::ItemPropertyType type = property.type.value();
        const QString name = property.name;
        const QString value = std::get<0>(property.values[0]);

        // Only some properties use ok2, so we can default it to true.
        bool ok = false;
        bool ok2 = true;

        // clang-format off
        switch (type) {
        case poe::ItemPropertyType::Level:                   i.gemLevel = value.section(' ', 0, 0).toInt(&ok); break;
        case poe::ItemPropertyType::Quality:                 i.quality  = value.sliced(1, value.size() - 2).toInt(&ok); break;
        case poe::ItemPropertyType::PhysicalDamage:          avg_phys_hit  = average(value, &ok); break;
        case poe::ItemPropertyType::ElementalDamage:         avg_ele_hit   = average(value, &ok); break;
        case poe::ItemPropertyType::ChaosDamage:             avg_chaos_hit = average(value, &ok); break;
        case poe::ItemPropertyType::CriticalStrikeChance:    i.weapon().criticalChance      = value.chopped(1).toFloat(&ok); break;
        case poe::ItemPropertyType::AttacksPerSecond:        i.weapon().attacksPerSecond    = value.toFloat(&ok); break;
        case poe::ItemPropertyType::WeaponRange:             i.weapon().weaponRange         = value.toFloat(&ok); break;
        case poe::ItemPropertyType::ChanceToBlock:           i.armour().block               = value.chopped(1).toInt(&ok); break;
        case poe::ItemPropertyType::Armour:                  i.armour().armour              = value.toInt(&ok); break;
        case poe::ItemPropertyType::EvasionRating:           i.armour().evasionRating       = value.toInt(&ok); break;
        case poe::ItemPropertyType::EnergyShield:            i.armour().energyShield        = value.toInt(&ok); break;
        case poe::ItemPropertyType::Ward:                    i.armour().ward                = value.toInt(&ok); break;
        case poe::ItemPropertyType::StackSize:               i.stackSize                    = value.section('/', 0, 0).toInt(&ok);
                                                             i.stackSizeMax                 = value.section('/', 1, 1).toInt(&ok2); break;
        case poe::ItemPropertyType::WingsRevealed:           i.heist().wingsRevealed        = value.section('/', 0, 0).toInt(&ok);
                                                             i.heist().wings                = value.section('/', 1, 1).toInt(&ok2); break;
        case poe::ItemPropertyType::EscapeRoutesRevealed:    i.heist().escapeRoutesRevealed = value.section('/', 0, 0).toInt(&ok);
                                                             i.heist().escapeRoutes         = value.section('/', 1, 1).toInt(&ok2); break;
        case poe::ItemPropertyType::RewardRoomsRevealed:     i.heist().wingsRevealed        = value.section('/', 0, 0).toInt(&ok);
                                                             i.heist().wings                = value.section('/', 1, 1).toInt(&ok2); break;
        // Heist requirements can appear in both the item properties and the item requirements.
        case poe::ItemPropertyType::LockpickingLevel:        i.heist().lockpickingLevel     = value.toInt(&ok); break;
        case poe::ItemPropertyType::BruteForceLevel:         i.heist().bruteForceLevel      = value.toInt(&ok); break;
        case poe::ItemPropertyType::PerceptionLevel:         i.heist().perceptionLevel      = value.toInt(&ok); break;
        case poe::ItemPropertyType::DemolutionLevel:         i.heist().demolutionLevel      = value.toInt(&ok); break;
        case poe::ItemPropertyType::CounterThaumaturgyLevel: i.heist().counterThaumaturgyLevel = value.toInt(&ok); break;
        case poe::ItemPropertyType::TrapDisarmamentLevel:    i.heist().trapDisarmamentLevel = value.toInt(&ok); break;
        case poe::ItemPropertyType::AgilityLevel:            i.heist().agilityLevel         = value.toInt(&ok); break;
        case poe::ItemPropertyType::DeceptionLevel:          i.heist().deceptionLevel       = value.toInt(&ok); break;
        case poe::ItemPropertyType::EngineeringLevel:        i.heist().engineeringLevel     = value.toInt(&ok); break;
        // Silently ignore all these properties:
        case poe::ItemPropertyType::MapTier:
        case poe::ItemPropertyType::ItemQuantity:
        case poe::ItemPropertyType::ItemRarity:
        case poe::ItemPropertyType::MonsterPackSize:
        case poe::ItemPropertyType::AreaLevel:
        case poe::ItemPropertyType::Radius:
        case poe::ItemPropertyType::HeistTarget:
        case poe::ItemPropertyType::Limit:
        case poe::ItemPropertyType::MoreMaps:
        case poe::ItemPropertyType::MoreScarabs:
        case poe::ItemPropertyType::BeastiaryGenus:
        case poe::ItemPropertyType::BeastiaryGroup:
        case poe::ItemPropertyType::BeastieryFamily:
        case poe::ItemPropertyType::UltimatumSacrifice:
        case poe::ItemPropertyType::UltimatumReward:
        case poe::ItemPropertyType::ValdoMapReward:
        case poe::ItemPropertyType::ValdoMapConvert:
        case poe::ItemPropertyType::ValdoShaperReward:   
        case poe::ItemPropertyType::ValdoElderReward:    
        case poe::ItemPropertyType::ValdoConquerorReward:
        case poe::ItemPropertyType::ValdoUniqueReward:    
        case poe::ItemPropertyType::ValdoScarabReward:
        case poe::ItemPropertyType::RitualVesselSource:
            ok = true;
            break;
        // These properties should not be present in this part of the item.
        case poe::ItemPropertyType::RequiredLevel:
        case poe::ItemPropertyType::RequiredStrength:
        case poe::ItemPropertyType::RequiredDexterity:
        case poe::ItemPropertyType::RequiredIntelligence:
        case poe::ItemPropertyType::RequiredClass:
        default:
            spdlog::error("Item a requirement in the properties section");
            ok = false;
            break;
        }
        // clang-format on

        if (!ok || !ok2) {
            spdlog::error("Error loading property {} of {}: '{}' from '{}'",
                          type,
                          i.prettyName,
                          name,
                          value);
        }
    }

    if (i.weaponData) {
        i.weaponData.value().physicalDps = avg_phys_hit * i.weaponData.value().attacksPerSecond;
        i.weaponData.value().elementalDps = avg_ele_hit * i.weaponData.value().attacksPerSecond;
        i.weaponData.value().chaosDps = avg_chaos_hit * i.weaponData.value().attacksPerSecond;
    }
}

void ItemData::loadRequirements(const poe::Item &item, ItemData &i)
{
    if (!item.requirements) {
        return;
    }

    for (const auto &requirement : item.requirements.value()) {
        // We only handle requirements with a type.
        if (!requirement.type) {
            continue;
        }

        const poe::ItemPropertyType type = requirement.type.value();
        const QString& name = requirement.name;
        const QString value = std::get<0>(requirement.values[0]);
        bool ok = false;

        // Parse the value as an integer because alll but one of the requirement attributes use this.
        const int k = value.toInt(&ok);

        // clang-format off
        switch (type) {
        case poe::ItemPropertyType::RequiredLevel:           i.requiredLevel                   = k; break;
        case poe::ItemPropertyType::RequiredStrength:        i.requiredStrength                = k; break;
        case poe::ItemPropertyType::RequiredDexterity:       i.requiredDexterity               = k; break;
        case poe::ItemPropertyType::RequiredIntelligence:    i.requiredIntelligence            = k; break;
        case poe::ItemPropertyType::RequiredClass:           i.requiredClass                   = value; ok = true; break;
        // Heist requirements can appear in both the item properties and the item requirements.
        case poe::ItemPropertyType::LockpickingLevel:        i.heist().lockpickingLevel        = k; break;
        case poe::ItemPropertyType::BruteForceLevel:         i.heist().bruteForceLevel         = k; break;
        case poe::ItemPropertyType::PerceptionLevel:         i.heist().perceptionLevel         = k; break;
        case poe::ItemPropertyType::DemolutionLevel:         i.heist().demolutionLevel         = k; break;
        case poe::ItemPropertyType::CounterThaumaturgyLevel: i.heist().counterThaumaturgyLevel = k; break;
        case poe::ItemPropertyType::TrapDisarmamentLevel:    i.heist().trapDisarmamentLevel    = k; break;
        case poe::ItemPropertyType::AgilityLevel:            i.heist().agilityLevel            = k; break;
        case poe::ItemPropertyType::DeceptionLevel:          i.heist().deceptionLevel          = k; break;
        case poe::ItemPropertyType::EngineeringLevel:        i.heist().engineeringLevel        = k; break;

        default:
            spdlog::warn("Unhandled requirement in {}: '{}' '{}'", i.prettyName, type, name);
            break;
        }
        // clang-format on

        if (!ok) {
            spdlog::error("Error loading requirement '{}' from '{}'", name, value);
        }
    }
}

QStringList ItemData::formatProperties(const poe::Item &item)
{
    if (!item.properties) {
        return {};
    }

    QStringList sections;
    QStringList lines;
    lines.reserve(item.properties.value().size());

    for (const auto &property : item.properties.value()) {
        if (property.type == poe::ItemPropertyType::ElementalDamage) {
            // Skip elemental damage since it's already in the explicit mods.
            continue;
        }
        if (property.displayMode == poe::DisplayMode::Separator) {
            if (!lines.isEmpty()) {
                sections.append(lines.join("<br>"));
                lines.clear();
            }
        } else {
            lines.append(property.render());
        }
    }

    if (!lines.isEmpty()) {
        sections.append(lines.join("<br>"));
    }

    return sections;
}

QString ItemData::formatRequirements(const poe::Item &item)
{
    QString result;

    // The first line is item level.
    if (item.ilvl > 0) {
        const poe::ItemProperty property{.name = "Item Level",
                                         .values = {{QString::number(item.ilvl), 0}},
                                         .displayMode = poe::DisplayMode::NameFirst,
                                         .type = poe::ItemPropertyType::Level};
        result = property.render();
    }

    // The second line is talisman tier.
    if (item.talismanTier) {
        const poe::ItemProperty property{.name = "Talisman Tier",
                                         .values = {{QString::number(item.talismanTier.value()), 0}},
                                         .displayMode = poe::DisplayMode::NameFirst,
                                         .type = poe::ItemPropertyType::Level};
        if (!result.isEmpty()) {
            result += "<br>";
        }
        result += property.render();
    }

    // Attribute requirements are all on the third line.
    if (item.requirements) {
        if (!result.isEmpty()) {
            result += "<br>";
        }

        const auto &requirements = item.requirements.value();
        QStringList parts;
        parts.reserve(requirements.size());
        for (const auto &requirement : requirements) {
            if (requirement.displayMode == poe::DisplayMode::Separator) {
                spdlog::error("Unexpected separator in item requirements");
            } else {
                parts.append(requirement.render());
            }
        }
        result += "Requires " + parts.join(", ");
    }
    return result;
}

QStringList ItemData::getMods(const std::optional<std::vector<QString>> mods)
{
    if (mods) {
        return QStringList(mods.value().begin(), mods.value().end());
    } else {
        return {};
    }
}

float ItemData::average(const QString &ranged_value, bool *ok)
{
    const QString left = ranged_value.section('-', 0, 0);
    const int min = left.toInt(ok);
    if (*ok) {
        const QString right = ranged_value.section('-', -1, -1);
        const float max = right.toFloat(ok);
        if (*ok) {
            return (min + max) / 2.0;
        }
    }
    return 0.0;
};

std::vector<ItemData::Column> ItemData::createColumnsInfo()
{
    std::vector<ItemData::Column> columns;

    // clang-format off
    
    // --- Basic fields ---
    columns.emplace_back("Name",           [](const ItemData &i) -> QVariant { return i.prettyName;    }); // could use i.Name
    columns.emplace_back("Type",           [](const ItemData &i) -> QVariant { return i.typeLine;      });
    columns.emplace_back("Category",       [](const ItemData &i) -> QVariant { return i.itemCategory;  });
    columns.emplace_back("Quality",        [](const ItemData &i) -> QVariant { return i.quality;       });
    columns.emplace_back("Item Level",     [](const ItemData &i) -> QVariant { return i.itemLevel;     });
    columns.emplace_back("Required Level", [](const ItemData &i) -> QVariant { return i.requiredLevel; });

    // --- Weapon fields ---
    columns.emplace_back("Damage",         [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->damage           : QVariant{""}; });
    columns.emplace_back("Crit Chance",    [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->criticalChance   : QVariant{""}; });
    columns.emplace_back("Phys DPS",       [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->physicalDps      : QVariant{""}; });
    columns.emplace_back("Ele DPS",        [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->elementalDps     : QVariant{""}; });
    columns.emplace_back("Chaos DPS",      [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->chaosDps         : QVariant{""}; });
    columns.emplace_back("Total DPS",      [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->totalDps         : QVariant{""}; });
    columns.emplace_back("APS",            [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->attacksPerSecond : QVariant{""}; });
    columns.emplace_back("Range",          [](const ItemData &i) -> QVariant { return i.weaponData ? i.weaponData->weaponRange      : QVariant{""}; });

    // --- Armour fields ---
    columns.emplace_back("Armour",         [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->armour         : QVariant{""}; });
    columns.emplace_back("Evasion",        [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->evasionRating  : QVariant{""}; });
    columns.emplace_back("Energy Shield",  [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->energyShield   : QVariant{""}; });
    columns.emplace_back("Block",          [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->block          : QVariant{""}; });
    columns.emplace_back("Ward",           [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->ward           : QVariant{""}; });
    columns.emplace_back("Base",           [](const ItemData &i) -> QVariant { return i.armourData ? i.armourData->baseBercentile : QVariant{""}; });

    // clang-format on

    return columns;
}
