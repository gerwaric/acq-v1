// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/passivegroup.h>
#include <poe/types/passivenode.h>

#include <QString>

#include <optional>
#include <unordered_map>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-ItemJewelData

    struct ItemJewelData
    {
        struct Subgraph
        {
            std::unordered_map<QString, poe::PassiveGroup>
                groups; // dictionary of PassiveGroup the key is the string value of the group id
            std::unordered_map<QString, poe::PassiveNode>
                nodes; // dictionary of PassiveNode the key is the string value of the node identifier
        };

        QString type;                                         // string
        std::optional<unsigned> radius;                       // ? uint
        std::optional<unsigned> radiusMin;                    // ? uint
        std::optional<QString> radiusVisual;                  // ? string
        std::optional<poe::ItemJewelData::Subgraph> subgraph; // ? object only present on cluster jewels
    };

} // namespace poe
