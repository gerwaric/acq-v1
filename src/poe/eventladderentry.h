// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-EventLadderEntry

    struct EventLadderEntry
    {
        struct PrivateLeague
        {
            QString name; // string
            QString url;  // string a url link to a Path of Exile Private League
        };

        unsigned rank;                  // uint
        std::optional<bool> ineligible; // ? bool
        std::optional<unsigned> time; // ? uint time taken to complete the league objective in seconds
        poe::EventLadderEntry::PrivateLeague private_league; // object
    };

} // namespace poe
