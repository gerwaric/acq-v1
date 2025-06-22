// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "model/characterinfo.h"

CharacterInfo::CharacterInfo(const poe::Character &character)
{
    id = character.id;
    name = character.name;
    realm = character.realm;
    league = character.league.value_or("");
}
