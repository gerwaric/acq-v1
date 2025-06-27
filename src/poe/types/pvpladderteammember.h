// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/account.h>

#include <glaze/glaze.hpp>

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-PvPLadderTeamMember

    struct PvPLadderTeamMember
    {
        struct Character
        {
            QString id;                    // string a unique 64 digit hexadecimal string
            QString name;                  // string
            unsigned level;                // uint
            QString class_;                // string
            std::optional<QString> league; // ? string
            std::optional<unsigned> score; // ? uint count of league objective completions
        };

        poe::Account account;                          // Account
        poe::PvPLadderTeamMember::Character character; // object
        std::optional<bool> public_;                   // ? bool always true if present
    };

} // namespace poe

template<>
struct glz::meta<poe::PvPLadderTeamMember::Character>
{
    // clang-format off
    using T = poe::PvPLadderTeamMember::Character;
    static constexpr auto value = glz::object(
        "id",     &T::id,
        "name",   &T::name,
        "level",  &T::level,
        "class",  &T::class_,
        "league", &T::league,
        "score",  &T::score
        );
    // clang-format on
};

template<>
struct glz::meta<poe::PvPLadderTeamMember>
{
    // clang-format off
    using T = poe::PvPLadderTeamMember;
    static constexpr auto value = glz::object(
        "account",   &T::account,
        "character", &T::character,
        "public",    &T::public_
        );
    // clang-format on
};
