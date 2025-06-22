// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only


#include "model/characternode.h"

#include "model/itemnode.h"
#include "model/rootnode.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

CharacterNode::CharacterNode(const poe::Character& character, TreeNode* parent)
    : TreeNode(NodeType::Character, character.name, parent)
    //, m_character(character)
{
	spdlog::trace("Creating CharacterNode for {} under {}", character.name, parent->name());

    if (character.equipment) {
        addCollection("Equipment", character.equipment.value());
    }
    if (character.inventory) {
        addCollection("Inventory", character.inventory.value());
    }
    if (character.rucksack) {
        addCollection("Rucksack", character.rucksack.value());
    }
    if (character.jewels) {
        addCollection("Jewels", character.jewels.value());
    }
}

void CharacterNode::addCollection(const QString& name, const std::vector<poe::Item>& items)
{
    addChild<RootNode>(name).addChildren<ItemNode>(items);
}
