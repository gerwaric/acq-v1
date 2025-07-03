// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/treenode.h"

std::atomic<long unsigned> TreeNode::s_node_count{0};

TreeNode::TreeNode(const QString &name, TreeNode *parent)
    : m_id{s_node_count++}
    , m_name{name}
    , m_parent{parent}
{}

TreeNode::TreeNode(const poe::Character &character, TreeNode *parent)
    : TreeNode{character.name, parent}
{
    m_payload = CharacterInfo{character};

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

TreeNode::TreeNode(const poe::StashTab &stash, TreeNode *parent)
    : TreeNode{stash.name, parent}
{
    m_payload = StashInfo{stash};

    if (stash.items) {
        addChildren(stash.items.value());
    }
}

TreeNode::TreeNode(const poe::Item &item, TreeNode *parent)
    : TreeNode{item.name + (item.name.isEmpty() ? "" : " ") + item.typeLine, parent}
{
    m_payload = ItemInfo{item};

    if (item.socketedItems) {
        addChildren(item.socketedItems.value());
    }
}

void TreeNode::addCollection(const QString &name, const std::vector<poe::Item> &items)
{
    if (!items.empty()) {
        auto &root = addChild(name);
        root.addChildren(items);
    }
}
