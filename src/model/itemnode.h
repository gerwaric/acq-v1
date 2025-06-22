// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"
#include "model/iteminfo.h"
#include "model/treenode.h"

class ItemNode : public TreeNode
{
public:
	explicit ItemNode(const poe::Item& item, TreeNode* parent);
    ~ItemNode() override {};

    //inline int columnCount() const override { return ItemInfo::ColumnCount; };
    QVariant data(int column) const override;

private:
    const ItemInfo m_itemInfo;
};
