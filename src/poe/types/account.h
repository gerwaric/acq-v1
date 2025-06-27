// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/guild.h>

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-Account

    struct Account
    {
        struct Challenges
        {
            QString set;        // string the challenge set
            unsigned completed; // uint
            unsigned max;       // uint
        };

        struct TwitchStream
        {
            QString name;   // string
            QString image;  // string
            QString status; // string
        };

        struct Twitch
        {
            QString name;                                     // string
            std::optional<poe::Account::TwitchStream> stream; // ? object
        };

        QString name;                                       // string
        std::optional<QString> realm;                       // ? string pc, xbox, or sony
        std::optional<poe::Guild> guild;                    // ? Guild
        std::optional<poe::Account::Challenges> challenges; // ? object
        std::optional<poe::Account::Twitch> twitch;         // ? object
    };
} // namespace poe
