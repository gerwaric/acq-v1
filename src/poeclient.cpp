// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "poeclient.h"

PoeClient::PoeClient(QObject *parent)
    : QObject(parent) {};

// https://www.pathofexile.com/developer/docs/reference#profile-get
void PoeClient::getProfile() {};

// https://www.pathofexile.com/developer/docs/reference#leagues-list
void PoeClient::listLeagues(QStringView realm, QStringView type, int limit, int offset) {};

// https://www.pathofexile.com/developer/docs/reference#characters-list
void PoeClient::listCharacters(QStringView realm) {};

// https://www.pathofexile.com/developer/docs/reference#stashes-list
void PoeClient::listStashes(QStringView realm, QStringView league) {};

// https://www.pathofexile.com/developer/docs/reference#characters-get
void PoeClient::getCharacter(QStringView realm, QStringView name) {};

// https://www.pathofexile.com/developer/docs/reference#stashes-get
void PoeClient::getStash(QStringView realm,
                         QStringView league,
                         QStringView stash_id,
                         QStringView substash_id) {};
