// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"

#include <QString>

struct CharacterInfo
{
    CharacterInfo(const poe::Character &character);

    QString id;
    QString name;
    QString realm;
    QString league;
};
