// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/treemodel.h"

#include "model/characternode.h"
#include "model/iteminfo.h"
#include "model/stashnode.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root("Root")
    , m_characters(m_root.addChild<RootNode>("Characters"))
    , m_stashes(m_root.addChild<RootNode>("Stash Tabs"))
{}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
    const TreeNode* parent_node = getNode(parent);
    if (parent_node) {
        const TreeNode* child_node = parent_node->child(row);
        if (child_node) {
            return createIndex(row, column, child_node);
        }
    }
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
    const TreeNode* child = getNode(index);
    if (child != &m_root) {
        const TreeNode* parent = child->parent();
        if (parent != &m_root) {
            return createIndex(parent->row(), 0, parent);
        }
    }
    return QModelIndex();
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole) {
        if ((section >= 0) && (section < ItemInfo::ColumnCount)) {
            return ItemInfo::Columns[section].header;
        }
    }
    return QVariant();
}

void TreeModel::appendCharacter(const poe::Character& character)
{
    const int k = m_characters.childCount();
    const QModelIndex parent_index = createIndex(0, 0, &m_characters);
    beginInsertRows(parent_index, k, k);
    m_characters.addChild<CharacterNode>(character);
    endInsertRows();
}

void TreeModel::appendStash(const poe::StashTab& stash)
{
    const int k = m_stashes.childCount();
    const QModelIndex parent_index = createIndex(0, 0, &m_stashes);
    beginInsertRows(parent_index, k, k);
    m_stashes.addChild<StashNode>(stash);
    endInsertRows();
}
