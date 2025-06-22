// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "model/stashinfo.h"
#include "model/treenode.h"
#include "poe.h"

class StashNode : public TreeNode
{
public:
	explicit StashNode(const poe::StashTab& stash, TreeNode* parent);
    ~StashNode() override {};
    QVariant data(int column) const override;
private:
	//const poe::StashTab& m_stash;
    std::unique_ptr<StashInfo> m_stashInfo;
};
