// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>

namespace poe {

    struct AccountProfile
    {
        struct Twitch
        {
            QString name; // string
        };

        QString uuid;                  // string UUIDv4 in canonical format
        QString name;                  // string
        std::optional<QString> locale; // ?string
        std::optional<Twitch> twitch;  // ?object present if the account is Twitch-linked
    };

} // namespace poe
