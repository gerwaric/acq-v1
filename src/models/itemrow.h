// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"

#include <QString>

struct ItemRow
{
    ItemRow(const poe::Item &item);

    // Basics
    QString id;
    QString name;
    poe::FrameType frameType;
    int itemLevel{0};

    // Requirements
    int requiredLevel{0};
    int requiredStr{0};
    int requiredDex{0};
    int requiredInt{0};

    // Defense
    int armour{0};
    int evasionRating{0};
    int energyShield{0};

    // Various
    bool identified{false};
    bool enchanted{false};
    bool corrupted{false};
    bool influenced{false};

private:
    void loadProperties(const std::vector<poe::ItemProperty> &properties);
    void loadRequirements(const std::vector<poe::ItemProperty> &requirements);

    static int getValue(const poe::ItemProperty &property);
};
