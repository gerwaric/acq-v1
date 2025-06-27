// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/enums.h>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-DisplayMode

    using DisplayMode = poe_enums::DisplayMode;
    /*
    enum class DisplayMode : unsigned {

        NameFirst = 0,      // Name should be followed by values
        ValuesFirst = 1,    // Values should be followed by name
        ProgressBar = 2,    // Progress bar
        InsertedValues = 3, // Values should be inserted into the string by index
        Separator = 4       // Separator
    };
    */

} // namespace poe
