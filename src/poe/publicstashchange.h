// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/item.h>

#include <glaze/glaze.hpp>

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-PublicStashChange

    struct PublicStashChange
    {
        QString id;                         // string a unique 64 digit hexadecimal string
        bool public_;                       // bool if false then optional properties will be null
        std::optional<QString> accountName; // ? string
        std::optional<QString> stash;       // ? string the name of the stash
        std::optional<QString>
            lastCharacterName; // ? string not included by default.Requires extra permissions
        QString stashType;     // string
        std::optional<QString> league; // ? string the league's name
        std::vector<poe::Item> items;  // array of Item
    };

} // namespace poe

template<>
struct glz::meta<poe::PublicStashChange>
{
    // clang-format off
    using T = poe::PublicStashChange;
    static constexpr auto value = glz::object(
        "id",                &T::id,
        "public",            &T::public_,
        "accountName",       &T::accountName,
        "stash",             &T::stash,
        "lastCharacterName", &T::lastCharacterName,
        "stashType",         &T::stashType,
        "league",            &T::league,
        "items",             &T::items
        );
    // clang-format on
};
