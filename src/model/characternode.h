// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"
#include "model/characterinfo.h"
#include "model/treenode.h"

class CharacterNode : public TreeNode
{
public:
	CharacterNode(const poe::Character& character, TreeNode* parent);
    ~CharacterNode() override {};

    inline int columnCount() const override { return 1; }

private:
	void addCollection(const QString& name, const std::vector<poe::Item>& items);
    const CharacterInfo m_characterInfo;
};
