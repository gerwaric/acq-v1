// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "datastore.h"
#include "poe.h"

#include <QByteArray>
#include <QObject>
#include <QString>

#include <vector>

class UserStore : public DataStore
{
    Q_OBJECT
public:
    explicit UserStore(const QString &username, QObject *parent = nullptr);

    QSqlDatabase getDatabase() { return getThreadLocalDatabase(); };

    void loadLeagueList(const QString &realm);
    void loadCharacterList(const QString &realm);
    void loadCharacters(const QString &realm, const QString &league);
    void loadStashList(const QString &realm, const QString &league);
    void loadStashes(const QString &realm, const QString &league);

signals:
    void leagueListReceived(std::vector<poe::League> leagueList);
    void characterListReceived(std::vector<poe::Character> characterList);
    void characterReceived(poe::Character character);
    void stashListReceived(std::vector<poe::StashTab> stashList);
    void stashReceived(poe::StashTab stash);

public slots:
    void saveLeagueList(const QString &realm,
                        const std::vector<poe::League> &leagues,
                        const QByteArray &data);

    void saveCharacterList(const QString &realm,
                           const std::vector<poe::Character> &characters,
                           const QByteArray &data);

    void saveStashList(const QString &realm,
                       const QString &league,
                       const std::vector<poe::StashTab> &stashes,
                       const QByteArray &data);

    void saveCharacter(const QString &realm,
                       const QString &name,
                       const std::optional<poe::Character> &character,
                       const QByteArray &data);

    void saveStash(const QString &realm,
                   const QString &league,
                   const QString &stash_id,
                   const QString &substash_id,
                   const std::optional<poe::StashTab> &stash,
                   const QByteArray &data);

private:
    void updateIndex(const QString &name,
                     const QString &realm,
                     const QString &league,
                     const QByteArray &data);

    static QString getPath(const QString &username);
};
