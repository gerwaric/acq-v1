// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/account.h>

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
