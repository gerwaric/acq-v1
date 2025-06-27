// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/item.h>

#include <vector>

namespace poe {

    struct WebCharacter
    {
        QString name;
        QString realm;
        QString class_;
        QString league;
        unsigned level;
        bool pinnable;
    };

    struct LegacyCharacterList
    {
        std::vector<poe::WebCharacter> characters;
    };

    struct LegacyCharacterStash
    {
        std::vector<poe::Item> items;
    };

} // namespace poe
