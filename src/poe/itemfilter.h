// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-ItemFilter

    struct ItemFilter
    {
        struct Validity
        {
            bool valid;                       // bool
            std::optional<QString> version;   // ? string game version
            std::optional<QString> validated; // ? string date time(ISO8601)
        };

        QString id;                    // string
        QString filter_name;           // string
        QString realm;                 // string
        QString description;           // string
        QString version;               // string
        QString type;                  // string either Normal or Ruthless
        bool public_;                  // ? bool always true if present
        std::optional<QString> filter; // ? string not present when listing all filters
        std::optional<poe::ItemFilter::Validity>
            validation; // ? object not present when listing all filters
    };

} // namespace poe
