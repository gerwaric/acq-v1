// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/itemnode.h"

ItemNode::ItemNode(const poe::Item &item, TreeNode *parent)
    : TreeNode(NodeType::Item, item.baseType, parent)
    , m_itemInfo(item)
{
    if (item.socketedItems) {
        addChildren<ItemNode>(item.socketedItems.value());
    }
}

QVariant ItemNode::data(int column) const
{
    const bool valid = (column >= 0) && (column < ItemInfo::ColumnCount);
    if (valid) {
        return ItemInfo::Columns[column].getter(m_itemInfo);
    } else {
        return QVariant();
    }
}
