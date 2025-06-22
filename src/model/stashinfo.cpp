// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/stashinfo.h"

StashInfo::StashInfo(poe::StashTab &stash)
{
    id = stash.id;
    parent = stash.parent;
    folder = stash.folder;
    name = stash.name;
    index = stash.index;

    if (stash.children) {
        children.reserve(stash.children->size());
        for (const auto &child : *stash.children) {
            children.emplace_back(child.id);
        }
    }
}
