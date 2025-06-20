// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "itemrow.h"

ItemRow::ItemRow(const poe::Item &item)
{
    // Basics
    id = item.id.value_or("");
    name = item.name;
    itemLevel = item.itemLevel.value_or(item.ilvl);
    frameType = item.frameType;

    if (item.properties) {
        loadProperties(*item.properties);
    };

    if (item.requirements) {
        loadRequirements(*item.requirements);
    };

    // Various
    identified = item.identified;
    enchanted = item.enchantMods.has_value();
    corrupted = item.corrupted.value_or(false);
    influenced = item.influences.has_value();
}

void ItemRow::loadProperties(const std::vector<poe::ItemProperty> &properties)
{
    for (const auto &property : properties) {
        if (property.type) {
            switch (*property.type) {
            case poe::ItemPropertyType::Armour:
                armour = getValue(property);
                break;
            case poe::ItemPropertyType::EvasionRating:
                evasionRating = getValue(property);
                break;
            case poe::ItemPropertyType::EnergyShield:
                energyShield = getValue(property);
            default:
                break;
            };
        };
    };
}

void ItemRow::loadRequirements(const std::vector<poe::ItemProperty> &requirements)
{
    for (const auto &requirement : requirements) {
        if (requirement.type) {
            switch (*requirement.type) {
            case poe::ItemPropertyType::RequiredLevel:
                requiredLevel = getValue(requirement);
                break;
            case poe::ItemPropertyType::RequiredStr:
                requiredStr = getValue(requirement);
                break;
            case poe::ItemPropertyType::RequiredDex:
                requiredDex = getValue(requirement);
                break;
            case poe::ItemPropertyType::RequiredInt:
                requiredInt = getValue(requirement);
                break;
            default:
                break;
            };
        };
    };
}

int ItemRow::getValue(const poe::ItemProperty &property)
{
    const auto [value, type] = property.values.front();
    return value.toInt();
}
