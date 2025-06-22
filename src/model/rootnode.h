// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "model/treenode.h"

class RootNode : public TreeNode
{
public:
    explicit RootNode(const QString &name, TreeNode *parent = nullptr);

    ~RootNode() override {};

    inline int columnCount() const override { return 1; }
};
