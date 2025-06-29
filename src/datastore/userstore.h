// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "datastore.h"
#include "poe/types/character.h"
#include "poe/types/league.h"
#include "poe/types/stashtab.h"
#include "poeclient.h"

#include <QByteArray>
#include <QObject>
#include <QSqlDatabase>
#include <QString>

#include <vector>

class UserStore : public DataStore
{
    Q_OBJECT
public:
    explicit UserStore(const QString &username, QObject *parent = nullptr);

    void connectTo(PoeClient &client);

    QSqlDatabase getDatabase() { return getThreadLocalDatabase(); }

    QStringList getLeagueNames(const QString &realm);

    std::vector<poe::League> getLeagueList(const QString &realm);
    std::vector<poe::Character> getCharacterList(const QString &realm);
    std::vector<poe::StashTab> getStashList(const QString &realm, const QString &league);

    std::optional<poe::Character> getCharacter(const QString &realm, const QString &name);
    std::optional<poe::StashTab> getStash(const QString &realm,
                                          const QString &league,
                                          const QString &id);

    void loadLeagueList(const QString &realm);
    void loadCharacterList(const QString &realm);
    void loadCharacters(const QString &realm, const QString &league);
    void loadStashList(const QString &realm, const QString &league);
    void loadStashes(const QString &realm, const QString &league);

signals:
    void leagueListReady(std::vector<poe::League> leagueList);
    void characterListReady(std::vector<poe::Character> characterList);
    void characterReady(poe::Character character);
    void stashListReady(const QString &realm,
                        const QString &league,
                        std::vector<poe::StashTab> stashList);
    void stashReady(const QString &realm, const QString &league, poe::StashTab stash);

public slots:
    void storeLeagueListData(const QString &realm, const QByteArray &data);

    void storeCharacterListData(const QString &realm, const QByteArray &data);

    void storeStashListData(const QString &realm, const QString &league, const QByteArray &data);

    void storeCharacterData(const QString &realm, const QString &name, const QByteArray &data);

    void storeStashData(const QString &realm,
                        const QString &league,
                        const QString &stash_id,
                        const QString &substash_id,
                        const QByteArray &data);

private:
    QByteArray getIndex(const QString &name, const QString &realm, const QString &league);
    void updateIndex(const QString &name,
                     const QString &realm,
                     const QString &league,
                     const QByteArray &data);

    static QString getPath(const QString &username);
};
