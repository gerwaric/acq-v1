// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

namespace poe {

    // https://www.pathofexile.com/developer/docs/reference#type-Guild

    struct Guild
    {
        unsigned id;  // uint
        QString name; // string
        QString tag;  // string
    };

} // namespace poe
