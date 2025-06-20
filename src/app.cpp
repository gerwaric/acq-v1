// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "models/itemrow.h"

#include "util/json.h"
#include "util/spdlog_qt.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

ACQUISITION_USE_SPDLOG

App::App(QObject *parent)
    : QObject(parent)
    , m_oauthManager(m_networkManager)
    , m_rateLimiter(m_networkManager)
{
    connect(&m_oauthManager, &OAuthManager::grantAccess, this, &App::accessGranted);
    connect(&m_poeClient, &PoeClient::sendRequest, &m_rateLimiter, &RateLimiter::makeRequest);
    connect(&m_rateLimiter, &RateLimiter::Paused, this, &App::rateLimited);

    const auto [token, ok] = m_globalStore.retrieve<OAuthToken>("oauth_token");
    if (ok) {
        m_oauthManager.setToken(token);
    }

    spdlog::info("App: creating items store");

    createItemsStore();

    QSqlDatabase db = QSqlDatabase::database("items_store");

    m_itemsModel = new QSqlTableModel(this, db);
    m_itemsModel->setTable("items");
    m_itemsModel->select();
    m_itemsModel->setHeaderData(0, Qt::Horizontal, "Armour");
    m_itemsModel->setHeaderData(1, Qt::Horizontal, "Evasion");
    m_itemsModel->setHeaderData(1, Qt::Horizontal, "Energy Shield");
}

void App::authenticate()
{
    m_oauthManager.initLogin();
}

void App::listLeagues()
{
    m_poeClient.listLeagues(m_realm);
}

void App::listCharacters()
{
    m_poeClient.listCharacters(m_realm);
}

void App::listStashes()
{
    m_poeClient.listStashes(m_realm, m_league);
}

void App::getCharacter()
{
    m_poeClient.getCharacter(m_realm, m_character);
}

void App::getAllCharacters()
{
    for (const auto &character : m_characterList) {
        m_poeClient.getCharacter(m_realm, character.name);
    }
}

void App::getAllStashes()
{
    for (const auto &stash : m_stashList) {
        m_poeClient.getStash(m_realm, m_league, stash.id, "");
    }
}

QStringList App::getLeagueNames() const
{
    spdlog::trace("App: getting league names");
    QStringList names;
    names.reserve(m_leagueList.size());
    for (const auto &league : m_leagueList) {
        names.append(league.name);
    }
    return names;
}

QStringList App::getCharacterNames() const
{
    spdlog::debug("App: getting character names");
    QStringList names;
    names.reserve(m_characterList.size());
    for (const auto &character : m_characterList) {
        names.append(character.name);
    }
    return names;
}

QStringList App::getStashNames() const
{
    QStringList names;
    names.reserve(m_stashList.size());
    for (const auto &stash : m_stashList) {
        names.append(stash.name);
    }
    return names;
}

void App::accessGranted(const OAuthToken &token)
{
    // First store the token.
    const QString json_token = json::toString(token);
    m_globalStore.set("oauth_token", json_token);

    // Then update ourselves.
    m_networkManager.setBearerToken(token.access_token);
    m_authenticated = true;
    m_username = token.username;

    if (m_userStore) {
        delete m_userStore;
    }
    m_userStore = new UserStore(m_username, this);

    connect(&m_poeClient, &PoeClient::leagueListReceived, m_userStore, &UserStore::handleLeagueList);
    connect(&m_poeClient,
            &PoeClient::characterListReceived,
            m_userStore,
            &UserStore::handleCharacterList);
    connect(&m_poeClient, &PoeClient::stashListReceived, m_userStore, &UserStore::handleStashList);
    connect(&m_poeClient, &PoeClient::characterReceived, m_userStore, &UserStore::handleCharacter);
    connect(&m_poeClient, &PoeClient::stashReceived, m_userStore, &UserStore::handleStash);

    connect(m_userStore, &UserStore::leagueListReceived, this, &App::handleLeagueList);
    connect(m_userStore, &UserStore::characterListReceived, this, &App::handleCharacterList);
    connect(m_userStore, &UserStore::stashListReceived, this, &App::handleStashList);
    connect(m_userStore, &UserStore::characterReceived, this, &App::handleCharacter);

    m_userStore->loadCharacterList(m_realm);
    m_userStore->loadLeagueList(m_realm);
    m_userStore->loadStashList(m_realm, m_league);
    m_userStore->loadStashes(m_realm, m_league);

    emit authenticationStateChanged();
}

void App::rateLimited(int seconds, const QString &policy_name)
{
    if (seconds <= 0) {
        m_rateLimitStatus = "Not rate limited.";
    } else {
        m_rateLimitStatus = QString("Rate limited for %1 seconds: %2")
                                .arg(QString::number(seconds), policy_name);
    }
    emit rateLimitStatusChanged();
}

void App::handleLeagueList(poe::LeagueListPtr leagues)
{
    if (!leagues) {
        spdlog::error("App: leagues list pointer is null");
        return;
    }
    m_leagueList = *leagues;
    emit leaguesUpdated();
}

void App::handleCharacterList(poe::CharacterListPtr characters)
{
    if (!characters) {
        spdlog::error("App: character list pointer is null");
        return;
    }
    m_characterList = *characters;
    emit charactersUpdated();
}

void App::handleCharacter(poe::CharacterPtr character)
{
    if (!character) {
        spdlog::error("App: character pointer is null");
        return;
    }
    if (character->inventory) {
        QSqlDatabase db = QSqlDatabase::database("items_store");
        QSqlQuery query(db);
        for (const auto &item : *character->inventory) {
            const QString pretty_name = item.name.isEmpty() ? item.typeLine
                                                            : item.name + " " + item.typeLine;
            spdlog::info("{}: {}", character->name, pretty_name);
            ItemRow row(item);
            query.prepare("INSERT INTO items (armour, evasion, energyShield) VALUES (?, ?, ?)");
            query.bindValue(0, row.armour);
            query.bindValue(1, row.evasionRating);
            query.bindValue(2, row.energyShield);
            if (!query.exec()) {
                const QString message = query.lastError().text();
                spdlog::error("App: error inserting item: {}", message);
                return;
            }
        }
    }
}

void App::handleStashList(poe::StashListPtr stashes)
{
    if (!stashes) {
        spdlog::error("App: stash list pointer is null");
        return;
    }
    m_stashList = *stashes;
    emit stashesUpdated();
}

void App::loadCurrentCharacter()
{
    m_userStore->loadCharacters(m_realm, m_league);
}

void App::createItemsStore()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "items_store");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        const QString message = db.lastError().text();
        spdlog::error("App: unable to create items store: {}", message);
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE items ("
                    "armour INTEGER,"
                    "evasion INTEGER,"
                    "energyShield INTEGER"
                    ")")) {
        const QString message = query.lastError().text();
        spdlog::error("App: error creating items store: {}", message);
    }
}
