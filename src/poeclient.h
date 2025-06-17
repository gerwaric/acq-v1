// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>

#include "poe/character.h"
#include "poe/league.h"
#include "poe/stashtab.h"

#include <vector>

class PoeClient : public QObject
{
    Q_OBJECT

public:
    explicit PoeClient(QObject *parent = nullptr);

    // https://www.pathofexile.com/developer/docs/reference#profile-get
    void getProfile();

    // https://www.pathofexile.com/developer/docs/reference#leagues-list
    void listLeagues(QStringView realm, QStringView type, int limit, int offset);

    // https://www.pathofexile.com/developer/docs/reference#characters-list
    void listCharacters(QStringView realm);

    // https://www.pathofexile.com/developer/docs/reference#stashes-list
    void listStashes(QStringView realm, QStringView league);

    // https://www.pathofexile.com/developer/docs/reference#characters-get
    void getCharacter(QStringView realm, QStringView name);

    // https://www.pathofexile.com/developer/docs/reference#stashes-get
    void getStash(QStringView realm,
                  QStringView league,
                  QStringView stash_id,
                  QStringView substash_id);

signals:
    void receivedProfile();
    void receivedLeagueList(std::vector<poe::League> leagues);
    void receivedCharacterList(std::vector<poe::Character> characters);
    void receivedStashList(std::vector<poe::StashTab> stash_tabs);
    void receivedCharacter(poe::Character character);
    void receivedStash(poe::StashTab stash_Tab);
};
