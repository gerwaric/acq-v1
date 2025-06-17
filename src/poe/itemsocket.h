// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-ItemSocket

    struct ItemSocket
    {
        unsigned group;                 // uint
        std::optional<QString> attr;    // ? string S, D, I, G, A, or DV
        std::optional<QString> sColour; // ? string R, G, B, W, A, or DV
    };

} // namespace poe
