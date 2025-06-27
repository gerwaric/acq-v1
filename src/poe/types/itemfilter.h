// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <glaze/glaze.hpp>

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

template<>
struct glz::meta<poe::ItemFilter>
{
    // clang-format off
    using T = poe::ItemFilter;
    static constexpr auto value = glz::object(
        "id",          &T::id,
        "filter_name", &T::filter_name,
        "realm",       &T::realm,
        "description", &T::description,
        "version",     &T::version,
        "type",        &T::type,
        "public",      &T::public_,
        "filter",      &T::filter,
        "validation",  &T::validation
        );
    // clang-format on
};
