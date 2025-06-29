// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/character.h>
#include <poe/types/league.h>
#include <poe/types/stashtab.h>

#include "datastore/userstore.h"
#include "poeclient.h"

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class PoeRepo : public QObject
{
    Q_OBJECT
public:
    PoeRepo(const QString &username, QObject *parent = nullptr);

    QSqlDatabase getDatabse() { return m_datastore.getDatabase(); }

    std::vector<poe::League> getLeagueList(const QString &realm);
    std::vector<poe::Character> getCharacterList(const QString &realm);
    std::optional<poe::Character> getCharacter(const QString &realm, const QString &name);
    std::vector<poe::StashTab> getStashList(const QString &realm, const QString &league);
    std::optional<poe::StashTab> getStash(const QString &realm,
                                          const QString &league,
                                          const QString &stash_id,
                                          const QString &substash_id);

    void updateLeageList(const QString &realm);
    void updateCharacterList(const QString &realm);
    void updateCharacter(const QString &realm, const QString &name);
    void updateStashList(const QString &realm, const QString &league);
    void updateStash(const QString &realm,
                     const QString &league,
                     const QString &stash_id,
                     const QString &substash_id);

    void loadCharacters(const QString &realm, const QString &league);
    void loadStashes(const QString &realm, const QString &league);

signals:
    void characterLoaded(QString realm, QString league, poe::Character character);
    void stashLoaded(QString realm, QString league, poe::StashTab stash);

private:
    PoeClient m_client;
    UserStore m_datastore;
};
