// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <poe/item.h>

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-StashTab

    struct StashTab
    {
        struct Metadata
        {
            std::optional<bool> public_; // ? bool always true if present
            std::optional<bool> folder;  // ? bool always true if present
            std::optional<QString>
                colour; // ? string 6 digit hex colour (NOTE: might be only 2 or 4 characters).
        };

        inline bool operator<(const StashTab &other) const
        {
            const unsigned a = index.value_or(0);
            const unsigned b = other.index.value_or(0);
            return a < b;
        };

        QString id;                       // string a 10 digit hexadecimal string
        std::optional<QString> parent;    // ? string a 10 digit hexadecimal string
        QString name;                     // string
        QString type;                     // string
        std::optional<unsigned> index;    // ? uint
        poe::StashTab::Metadata metadata; // metadata object
        std::optional<std::vector<poe::StashTab>> children; // ? array of StashTab
        std::optional<std::vector<poe::Item>> items;        // ? array of Item
    };

    struct StashListWrapper
    {
        std::vector<poe::StashTab> stashes;
    };

    struct StashWrapper
    {
        std::optional<poe::StashTab> stash;
    };

}; // namespace poe
