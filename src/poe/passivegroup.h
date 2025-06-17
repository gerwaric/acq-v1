// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-PassiveGroup

    struct PassiveGroup
    {
        float x;                      // float
        float y;                      // float
        std::vector<unsigned> orbits; //	array of uint
        std::optional<bool> isProxy;  // ? bool always true if present
        std::optional<QString> proxy; // ? string identifier of the placeholder node
        std::vector<QString> nodes; // array of string the node identifiers associated with this group;
    };

} // namespace poe
