// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/leaguerule.h>

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-League

    struct League
    {
        struct Category
        {
            QString id; // string the league category, e.g.Affliction
            std::optional<bool>
                current; // ? bool set for the active challenge leagues; always true if present
        };

        QString id;                                        // string the league's name
        QString name;                                      // TODO: Undocumented!
        std::optional<QString> realm;                      // ? string pc, xbox, or sony
        std::optional<QString> description;                // ? string
        std::optional<poe::League::Category> category;     // ? object
        std::optional<std::vector<poe::LeagueRule>> rules; // ? array of LeagueRule
        std::optional<QString> registerAt;                 // ? string date time(ISO8601)
        QString goal;                                      // TODO: undocumented!
        std::optional<bool> event;                         // ? bool always true if present
        std::optional<QString> url;        // ? string a url link to a Path of Exile forum thread
        std::optional<QString> startAt;    // ? string date time(ISO8601)
        std::optional<QString> endAt;      // ? string date time(ISO8601)
        std::optional<bool> timedEvent;    // ? bool always true if present
        std::optional<bool> scoreEvent;    // ? bool always true if present
        std::optional<bool> delveEvent;    // ? bool always true if present
        std::optional<bool> ancestorEvent; // ? bool always true if present
        std::optional<bool> leagueEvent;   // ? bool always true if present
    };

    struct LeagueListWrapper
    {
        std::vector<poe::League> leagues;
    };

} // namespace poe
