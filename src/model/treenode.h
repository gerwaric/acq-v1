// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "characterinfo.h"
#include "iteminfo.h"
#include "stashinfo.h"

#include "util/spdlog_qt.h"

#include <QString>
#include <QVariant>

#include <variant>
#include <vector>

class TreeNode {
public:
    using Payload = std::variant<std::monostate, CharacterInfo, StashInfo, ItemInfo>;

    explicit TreeNode(const QString &name, TreeNode *parent);

    template<typename T>
    inline constexpr bool hasPayload() const
    {
        return std::holds_alternative<T>(m_payload);
    }

    inline constexpr bool isRoot() const { return hasPayload<std::monostate>(); }
    inline constexpr bool isCharacter() const { return hasPayload<CharacterInfo>(); }
    inline constexpr bool isStash() const { return hasPayload<StashInfo>(); }
    inline constexpr bool isItem() const { return hasPayload<ItemInfo>(); }

    template<typename T>
    TreeNode &addChild(const T &object)
    {
        auto child = std::unique_ptr<TreeNode>{nullptr};
        child.reset(new TreeNode{object, this});
        m_children.emplace_back(std::move(child));
        return *m_children.back();
    }

    inline QString name() const { return m_name; }
    inline TreeNode *parent() const { return m_parent; }
    inline TreeNode *child(int row) const
    {
        const bool valid = (row >= 0) && (static_cast<size_t>(row) < m_children.size());
        return valid ? m_children[row].get() : nullptr;
    }
    inline bool hasChildren() const { return !m_children.empty(); }
    inline int childCount() const { return static_cast<int>(m_children.size()); }
    inline int row() const { return m_parent ? m_parent->rowOfChild(this) : 0; }
    inline int columnCount() const { return isItem() ? ItemInfo::ColumnCount : 1; }

    QVariant data(int column) const
    {
        if (isItem()) {
            if ((column >= 0) && (column < ItemInfo::ColumnCount)) {
                const auto &column_info = ItemInfo::Columns[column];
                const auto item_info = std::get<ItemInfo>(m_payload);
                return column_info.getter(item_info);
            }
        }
        return (column == 0) ? m_name : "";
    }

private:
    explicit TreeNode(const poe::Character &character, TreeNode *parent);
    explicit TreeNode(const poe::StashTab &stash, TreeNode *parent);
    explicit TreeNode(const poe::Item &item, TreeNode *parent);

    inline int rowOfChild(const TreeNode *child) const
    {
        for (auto i = 0; i < m_children.size(); ++i) {
            if (m_children[i].get() == child) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    template<typename T>
    void addChildren(const std::vector<T> &objects)
    {
        for (const auto &object : objects) {
            addChild(object);
        }
    }

    void addCollection(const QString &name, const std::vector<poe::Item> &items);

    const long unsigned m_id;
    const QString m_name;
    TreeNode *m_parent;
    std::vector<std::unique_ptr<TreeNode>> m_children;
    Payload m_payload;

    static QString getName(const Payload &payload);

    static std::atomic<long unsigned> s_node_count;
};
