// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/stashnode.h"

#include "model/itemnode.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

StashNode::StashNode(const poe::StashTab& stash, TreeNode* parent)
    : TreeNode(NodeType::Stash, stash.name, parent)
    //, m_stash(stash)
{
	spdlog::trace("Creating StashNode for {} under {}", stash.name, parent->name());

    if (stash.children) {
        addChildren<StashNode>(stash.children.value());
    }
    if (stash.items) {
        addChildren<ItemNode>(stash.items.value());
    }
}

QVariant StashNode::data(int column) const
{
    if (column > 0) {
        return "";
    } else if (m_stashInfo->index) {
        return QString("#%1, %2").arg(QString::number(*m_stashInfo->index), m_stashInfo->name);
    } else {
        return m_stashInfo->name;
    }
}
