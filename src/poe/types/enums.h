// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG);

#include <QObject>

// These have to be declared in one space because Qt does
// not support spreading namespaces out over multiple files.

namespace poe_enums {
    Q_NAMESPACE

    enum class DisplayMode : unsigned {
        NameFirst = 0,      // Name should be followed by values
        ValuesFirst = 1,    // Values should be followed by name
        ProgressBar = 2,    // Progress bar
        InsertedValues = 3, // Values should be inserted into the string by index
        Separator = 4       // Separator
    };
    Q_ENUM_NS(DisplayMode)

    enum class FrameType : unsigned {
        Normal = 0,
        Magic = 1,
        Rare = 2,
        Unique = 3,
        Gem = 4,
        Currency = 5,
        DivinationCard = 6,
        Quest = 7,
        Prophecy = 8,
        Foil = 9,
        SupporterFoil = 10,
        Necropolis = 11
    };
    Q_ENUM_NS(FrameType)

    // This is not an official enum, but it's helpful in catching
    // errors in the json parsing.
    enum class ItemPropertyType : unsigned {
        MapTier = 1,
        ItemQuantity = 2,
        ItemRarity = 3,
        MonsterPackSize = 4,
        Level = 5,
        Quality = 6,
        PhysicalDamage = 9,
        ElementalDamage = 10,
        ChaosDamage = 11,
        CriticalStrikeChance = 12,
        AttacksPerSecond = 13,
        WeaponRange = 14,
        ChanceToBlock = 15,
        Armour = 16,
        EvasionRating = 17,
        EnergyShield = 18,
        BeastiaryGenus = 21,
        BeastiaryGroup = 22,
        BeastieryFamily = 23,
        Radius = 24,
        StackSize = 32,
        AreaLevel = 34,
        WingsRevealed = 35,
        EscapeRoutesRevealed = 36,
        RewardRoomsRevealed = 37,
        LockpickingLevel = 38,
        BruteForceLevel = 39,
        PerceptionLevel = 40,
        DemolutionLevel = 41,
        CounterThaumaturgyLevel = 42,
        TrapDisarmamentLevel = 43,
        AgilityLevel = 44,
        DeceptionLevel = 45,
        EngineeringLevel = 46,
        HeistTarget = 47,
        RitualVesselSource = 48,
        UltimatumSacrifice = 51,
        UltimatumReward = 52,
        Ward = 54,
        RequiredClass = 57,
        RequiredLevel = 62,
        RequiredStrength = 63,
        RequiredDexterity = 64,
        RequiredIntelligence = 65,
        ValdoMapReward = 76,
        ValdoMapConvert = 77,
        ValdoShaperReward = 80,    // What is this?
        ValdoElderReward = 81,     // What is this?
        ValdoConquerorReward = 82, // What is this?
        ValdoUniqueReward = 83,    // What is this?
        ValdoScarabReward = 84,    // What is this?
        Limit = 87,
        MoreMaps = 88,
        MoreScarabs = 89
    };
    Q_ENUM_NS(ItemPropertyType)

}

template<>
struct fmt::formatter<poe_enums::DisplayMode, char> : QtEnumFormatter<poe_enums::DisplayMode>
{};

template<>
struct fmt::formatter<poe_enums::FrameType, char> : QtEnumFormatter<poe_enums::FrameType>
{};

template<>
struct fmt::formatter<poe_enums::ItemPropertyType, char>
    : QtEnumFormatter<poe_enums::ItemPropertyType>
{};
