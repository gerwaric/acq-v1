// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "poerepo.h"

PoeRepo::PoeRepo(const QString &username, QObject *parent)
    : QObject(parent)
    , m_client(this)
    , m_datastore(username, this)
{
    connect(&m_client,
            &PoeClient::leagueListDataReceived,
            &m_datastore,
            &UserStore::storeLeagueListData);

    connect(&m_client,
            &PoeClient::characterListDataReceived,
            &m_datastore,
            &UserStore::storeCharacterListData);

    connect(&m_client,
            &PoeClient::characterDataReceived,
            &m_datastore,
            &UserStore::storeCharacterData);

    connect(&m_client,
            &PoeClient::stashListDataReceived,
            &m_datastore,
            &UserStore::storeStashListData);

    connect(&m_client, &PoeClient::stashDataReceived, &m_datastore, &UserStore::storeStashData);
}

void PoeRepo::updateLeageList(const QString &realm)
{
    m_client.listLeagues(realm);
}

void PoeRepo::updateCharacterList(const QString &realm)
{
    m_client.listCharacters(realm);
}

void PoeRepo::updateCharacter(const QString &realm, const QString &name)
{
    m_client.getCharacter(realm, name);
}

void PoeRepo::updateStashList(const QString &realm, const QString &league)
{
    m_client.listStashes(realm, league);
}

void PoeRepo::updateStash(const QString &realm,
                          const QString &league,
                          const QString &stash_id,
                          const QString &substash_id)
{
    m_client.getStash(realm, league, stash_id, substash_id);
}

std::vector<poe::League> PoeRepo::getLeagueList(const QString &realm)
{
    return m_datastore.getLeagueList(realm);
}
