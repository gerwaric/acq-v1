// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "datastore.h"
#include "poe.h"

#include <QObject>

class UserStore : public DataStore
{
    Q_OBJECT
public:
    explicit UserStore(const QString &username, QObject *parent = nullptr);

    void loadLeagueList(const QString &realm);
    void loadCharacterList(const QString &realm);
    void loadCharacters(const QString &realm, const QString &league);
    void loadStashList(const QString &realm, const QString &league);
    void loadStashes(const QString &realm, const QString &league);

signals:
    void leagueListReceived(poe::LeagueListPtr leagues);
    void characterListReceived(poe::CharacterListPtr characters);
    void characterReceived(poe::CharacterPtr);
    void stashListReceived(poe::StashListPtr);
    void stashTabReceived(poe::StashTabPtr);

public slots:
    void handleLeagueList(QString realm, std::shared_ptr<QByteArray> data);
    void handleCharacterList(QString realm, std::shared_ptr<QByteArray> data);
    void handleStashList(QString realm, QString league, std::shared_ptr<QByteArray> data);
    void handleCharacter(QString realm, QString name, std::shared_ptr<QByteArray> data);
    void handleStash(QString realm,
                     QString league,
                     QString stash_id,
                     QString substash_id,
                     std::shared_ptr<QByteArray> data);

private:
    void updateIndex(const QString &name,
                     const QString &realm,
                     const QString &league,
                     const QByteArray &data);

    static QString getPath(const QString &username);
};
