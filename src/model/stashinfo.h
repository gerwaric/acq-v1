// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"

#include <QString>

#include <optional>

struct StashInfo
{
    StashInfo(poe::StashTab &stash);

    QString id;
    std::optional<QString> parent;
    std::optional<QString> folder;
    QString name;
    QString type;
    std::optional<unsigned> index;
    std::vector<QString> children;
};
