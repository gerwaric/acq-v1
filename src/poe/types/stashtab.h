// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

#include <poe/types/item.h>

#include <glaze/glaze.hpp>

#include <QString>

#include <optional>
#include <vector>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-StashTab

    struct StashTab
    {
        struct Metadata
        {
            struct Map
            {
                QString section;
                QString name;
                QString image;
                int tier;
                int series;
                int index;
            };

            std::optional<bool> public_; // ? bool always true if present
            std::optional<bool> folder;  // ? bool always true if present
            std::optional<QString>
                colour; // ? string 6 digit hex colour (NOTE: might be only 2 or 4 characters).
            glz::raw_json layout;             // TODO: undocumented!
            std::optional<int> items;         // TODO: undocumented!
            poe::StashTab::Metadata::Map map; // TODO: undocumented!
        };

        inline bool operator<(const StashTab &other) const
        {
            const unsigned a = index.value_or(0);
            const unsigned b = other.index.value_or(0);
            return a < b;
        };

        QString id;                       // string a 10 digit hexadecimal string
        std::optional<QString> folder;    // ? string a 10 digit hexadecimal string
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

template<>
struct glz::meta<poe::StashTab::Metadata>
{
    // clang-format off
    static constexpr auto value = glz::object(
        "public", &poe::StashTab::Metadata::public_,
        "folder", &poe::StashTab::Metadata::folder,
        "colour", &poe::StashTab::Metadata::colour,
        "layout", &poe::StashTab::Metadata::layout,
        "items",  &poe::StashTab::Metadata::items,
        "map",    &poe::StashTab::Metadata::map
    );
    // clang-format on
};
