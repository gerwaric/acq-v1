// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <optional>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-LeagueRule

    struct LeagueRule
    {
        QString id;                         // string examples : Hardcore, NoParties(SSF)
        QString name;                       // string
        std::optional<QString> description; // ? string
    };

} // namespace poe
