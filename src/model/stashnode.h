// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "model/stashinfo.h"
#include "model/treenode.h"

class StashNode : public TreeNode
{
public:
	explicit StashNode(const poe::StashTab& stash, TreeNode* parent);
    ~StashNode() override {};

    inline int columnCount() const override { return 1; }
    QVariant data(int column) const override;

private:
    const StashInfo m_stashInfo;
};
