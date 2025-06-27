// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-LeagueAccount

    struct LeagueAccount
    {
        struct AtlasPassiveTree
        {
            QString name;                 // string
            std::vector<unsigned> hashes; // array of uint
        };

        std::vector<poe::LeagueAccount::AtlasPassiveTree> atlas_passive_trees; // array of object
    };

} // namespace poe
