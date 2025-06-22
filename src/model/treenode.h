// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "util/spdlog_qt.h"

#include <QString>
#include <QVariant>

#include <vector>

class TreeNode {
public:
    explicit TreeNode(const QString &name, TreeNode *parent)
        : m_id(s_node_count++)
        , m_name(name)
        , m_parent(parent) {};

    virtual ~TreeNode() {};

    inline const QString &name() const { return m_name; }

    inline const TreeNode *parent() const { return m_parent; }

    inline const TreeNode *child(int row) const
    {
        return ((row >= 0) && (static_cast<size_t>(row) < m_children.size())) ? m_children[row].get() : nullptr;
    }

    inline int childCount() const { return static_cast<int>(m_children.size()); }

    inline int row() const { return m_parent ? m_parent->rowOfChild(this) : 0; }

    virtual inline int columnCount() const { return 1; }

    virtual inline QVariant data(int column) const { return (column == 0) ? m_name : ""; }

    inline bool hasChildren() const { return !m_children.empty(); }

    template<class Node, class Object>
    inline TreeNode &addChild(const Object &object)
    {
        return *m_children.emplace_back(std::make_unique<Node>(object, this));
    }

    template<class Node, class Object>
    inline void addChildren(const std::vector<Object> &objects)
    {
        for (const Object &object : objects) {
            addChild<Node>(object);
        }
    }

private:
    inline int rowOfChild(const TreeNode *child) const
    {
        for (int i = 0; i < static_cast<int>(m_children.size()); ++i) {
            if (m_children[i].get() == child) {
                return i;
            }
        }
        return -1;
    }

    const long unsigned m_id;
    const QString m_name;
    const TreeNode* m_parent;
    std::vector<std::unique_ptr<TreeNode>> m_children;

    static std::atomic<long unsigned> s_node_count;
};
