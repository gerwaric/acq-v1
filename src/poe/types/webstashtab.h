// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <types/item.h>

#include <optional>
#include <vector>

namespace poe {

    struct WebStashTab
    {
        struct Colour
        {
            unsigned r;
            unsigned g;
            unsigned b;
        };

        struct Tab
        {
            QString n;
            unsigned i;
            QString id;
            QString type;
            bool selected;
            poe::WebStashTab::Colour colour;
            QString srcL;
            QString srcC;
            QString srcR;
        };

        unsigned numTabs;
        std::vector<poe::WebStashTab::Tab> tabs;
        std::optional<bool> quadLayout;
        std::vector<poe::Item> items;
    };

} // namespace poe
