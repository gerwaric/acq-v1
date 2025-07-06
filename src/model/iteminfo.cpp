// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/iteminfo.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG);

ItemInfo::ItemInfo(const poe::Item& item)
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

void ItemInfo::loadSockets(const poe::Item &item, ItemInfo &i)
{
    if (!item.sockets) {
        return;
    }
    for (const auto& socket : item.sockets.value()) {
        const unsigned int group = socket.group;
        while (group >= i.socketGroups.size()) {
            i.socketGroups.push_back(SocketGroup());
        }
        const QString& colour = socket.sColour.value_or("");
        // clang-format off
        if      (colour == "R")  { ++i.redSockets;   ++i.socketGroups[group].red;   }
        else if (colour == "G")  { ++i.greenSockets; ++i.socketGroups[group].green; }
        else if (colour == "B")  { ++i.blueSockets;  ++i.socketGroups[group].blue;  }
        else if (colour == "W")  { ++i.whiteSockets; ++i.socketGroups[group].white; }
        else if (colour == "A")  { ++i.abyssSockets; ++i.socketGroups[group].abyss; }
        else if (colour == "DV") { /* ignore resonator sockets*/ }
        else {
            spdlog::error("Unhandled item socket in {} {}: {}", item.name, item.typeLine, colour);
        }
        // clang-format on
        ++i.totalSockets;
    }
}

void ItemInfo::loadProperties(const poe::Item &item, ItemInfo &i)
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
        bool ok = false;

        // clang-format off
        switch (type) {
        case poe::ItemPropertyType::Level:                i.gemLevel = value.section(' ', 0, 0).toInt(&ok); break;
        case poe::ItemPropertyType::Quality:              i.quality = value.sliced(1, value.size() - 2).toInt(&ok); break;
        case poe::ItemPropertyType::PhysicalDamage:       avg_phys_hit = average(value, &ok); break;
        case poe::ItemPropertyType::ElementalDamage:      avg_ele_hit = average(value, &ok); break;
        case poe::ItemPropertyType::ChaosDamage:          avg_chaos_hit = average(value, &ok); break;
        case poe::ItemPropertyType::CriticalStrikeChance: i.criticalChance = value.chopped(1).toFloat(&ok); break;
        case poe::ItemPropertyType::AttacksPerSecond:     i.attacksPerSecond = value.toFloat(&ok); break;
        case poe::ItemPropertyType::ChanceToBlock:        i.block = value.chopped(1).toInt(&ok); break;
        case poe::ItemPropertyType::Armour:               i.armour = value.toInt(&ok); break;
        case poe::ItemPropertyType::EvasionRating:        i.evasionRating = value.toInt(&ok); break;
        case poe::ItemPropertyType::EnergyShield:         i.energyShield = value.toInt(&ok); break;
        case poe::ItemPropertyType::Ward:                 i.ward = value.toInt(&ok); break;
        case poe::ItemPropertyType::StackSize:            i.stackSize = value.section('/', 0, 0).toInt(&ok); break;
        // Silently ignore all these properties:
        case poe::ItemPropertyType::MapTier:
        case poe::ItemPropertyType::ItemQuantity:
        case poe::ItemPropertyType::ItemRarity:
        case poe::ItemPropertyType::MonsterPackSize:
        case poe::ItemPropertyType::AreaLevel:
        case poe::ItemPropertyType::WingsRevealed:
        case poe::ItemPropertyType::Radius:
        case poe::ItemPropertyType::RequiredLockingLevel:
        case poe::ItemPropertyType::RequiredBruteForceLevel:
        case poe::ItemPropertyType::RequiredPerceptionLevel:
        case poe::ItemPropertyType::RequiredDemolutionLevel:
        case poe::ItemPropertyType::RequiredCounterThaumaturgyLevel:
        case poe::ItemPropertyType::RequiredTrapDisarmamentLevel:
        case poe::ItemPropertyType::RequiredAgilityLevel:
        case poe::ItemPropertyType::RequiredDeceptionLevel:
        case poe::ItemPropertyType::RequiredEngineeringLevel:
        case poe::ItemPropertyType::HeistTarget:
        case poe::ItemPropertyType::EscapeRoutesRevealed:
        case poe::ItemPropertyType::RewardRoomsRevealed:
        case poe::ItemPropertyType::WeaponRange:
        case poe::ItemPropertyType::Limit:
        case poe::ItemPropertyType::MoreMaps:
        case poe::ItemPropertyType::MoreScarabs:
        case poe::ItemPropertyType::RequiredLevel:
        case poe::ItemPropertyType::RequiredStrength:
        case poe::ItemPropertyType::RequiredDexterity:
        case poe::ItemPropertyType::RequiredIntelligence:
        case poe::ItemPropertyType::RequiredClass:
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
        }
        // clang-format on

        if (!ok) {
            spdlog::error("Error loading property of {}: '{}' from '{}'", i.prettyName, name, value);
        }
    }

    i.physicalDps = avg_phys_hit * i.attacksPerSecond;
    i.elementalDps = avg_ele_hit * i.attacksPerSecond;
    i.chaosDps = avg_chaos_hit * i.attacksPerSecond;
}

void ItemInfo::loadRequirements(const poe::Item &item, ItemInfo &i)
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

        const int k = value.toInt(&ok);

        // clang-format off
        switch (type) {
        case poe::ItemPropertyType::RequiredLevel:                   i.requiredLevel = k; break;
        case poe::ItemPropertyType::RequiredStrength:                i.requiredStrength = k; break;
        case poe::ItemPropertyType::RequiredDexterity:               i.requiredDexterity = k; break;
        case poe::ItemPropertyType::RequiredIntelligence:            i.requiredIntelligence = k; break;
        case poe::ItemPropertyType::RequiredLockingLevel:            i.requiredLockingLevel = k; break;
        case poe::ItemPropertyType::RequiredBruteForceLevel:         i.requiredBruteForceLevel = k; break;
        case poe::ItemPropertyType::RequiredPerceptionLevel:         i.requiredPerceptionLevel = k; break;
        case poe::ItemPropertyType::RequiredDemolutionLevel:         i.requiredDemolutionLevel = k; break;
        case poe::ItemPropertyType::RequiredCounterThaumaturgyLevel: i.requiredCounterThaumaturgyLevel = k; break;
        case poe::ItemPropertyType::RequiredTrapDisarmamentLevel:    i.requiredTrapDisarmamentLevel = k; break;
        case poe::ItemPropertyType::RequiredAgilityLevel:            i.requiredAgilityLevel = k; break;
        case poe::ItemPropertyType::RequiredDeceptionLevel:          i.requiredDeceptionLevel = k; break;
        case poe::ItemPropertyType::RequiredEngineeringLevel:        i.requiredEngineeringLevel = k; break;
        case poe::ItemPropertyType::RequiredClass:
            // Ignore required class.
            ok = true;
            break;
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

float ItemInfo::average(const QString &ranged_value, bool *ok)
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
