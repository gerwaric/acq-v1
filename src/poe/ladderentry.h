// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/account.h>

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-LadderEntry

    struct LadderEntry
    {
        struct DelveInfo
        {
            std::optional<unsigned> default_; // ? uint
            std::optional<unsigned> solo;     // ? uint
        };

        struct Character
        {
            QString id;     // string a unique 64 digit hexadecimal string
            QString name;   // string
            unsigned level; // uint
            QString class_; // string
            std::optional<unsigned>
                time; // ? uint time taken to complete the league objective in seconds
            std::optional<unsigned> score; // ? uint count of league objective completions
            // TBD progress; // ? object the values of this depend on the league objective
            std::optional<unsigned> experience;               // ? uint
            std::optional<poe::LadderEntry::DelveInfo> depth; // ? object deepest Delve depth completed
        };

        unsigned rank;                         // uint
        std::optional<bool> dead;              // ? bool
        std::optional<bool> retired;           // ? bool
        std::optional<bool> ineligible;        // ? bool
        std::optional<bool> public_;           // ? bool
        poe::LadderEntry::Character character; // object
        std::optional<poe::Account> account;   // ? Account
    };

} // namespace poe
