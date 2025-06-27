// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/displaymode.h>
#include <poe/types/enums.h>

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-ItemProperty

    struct ItemProperty
    {
        using Type = poe_enums::ItemPropertyType;

        QString name;                                 // string
        std::vector<std::tuple<QString, int>> values; // array of array
        std::optional<poe::DisplayMode> displayMode;  // ? uint as DisplayMode
        std::optional<double> progress;               // ? double rounded to 2 decimal places
        std::optional<ItemProperty::Type> type;       // ? uint
        std::optional<QString> suffix;                // ? string
    };

} // namespace poe
