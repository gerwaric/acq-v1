// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-FrameType

    enum class FrameType : unsigned {
        Normal = 0,
        Magic = 1,
        Rare = 2,
        Unique = 3,
        Gem = 4,
        Currency = 5,
        DivinationCard = 6,
        Quest = 7,
        Prophecy = 8,
        Foil = 9,
        SupporterFoil = 10,
        Necropolis = 11
    };

} // namespace poe
