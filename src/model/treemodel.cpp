// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/treemodel.h"
#include "model/treenode.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel{parent}
    , m_root{"Root", nullptr}
    , m_characterRoot{m_root.addChild("Characters")}
    , m_stashRoot{m_root.addChild("Stash Tabs")}
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
            return ItemInfo::Columns[section].name;
        }
    }
    return QVariant();
}

void TreeModel::addCharacter(const poe::Character &character)
{
    const int k = m_characterRoot.childCount();
    const QModelIndex index = createIndex(0, 0, &m_characterRoot);

    beginInsertRows(index, k, k);
    m_characterRoot.addChild(character);
    endInsertRows();
}

void TreeModel::addStash(const poe::StashTab &stash)
{
    const int k = m_stashRoot.childCount();
    const QModelIndex index = createIndex(0, 0, &m_stashRoot);

    beginInsertRows(index, k, k);
    m_stashRoot.addChild(stash);
    endInsertRows();
}
