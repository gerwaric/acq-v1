// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "userstore.h"

#include "util/json.h"
#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

UserStore::UserStore(const QString &username, QObject *parent)
    : DataStore(getPath(username), parent)
{
    createTable("indexes",
                {"name TEXT PRIMARY KEY",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp TEXT",
                 "data TEXT"});

    createTable("characters",
                {"id TEXT PRIMARY KEY",
                 "name TEXT",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp TEXT",
                 "data BLOB"});

    createIndexes("characters", {"realm", "league"});

    createTable("stashes",
                {"id TEXT PRIMARY KEY",
                 "parent TEXT REFERENCES stashes(id)",
                 "name TEXT",
                 "type TEXT",
                 "stash_index INTEGER",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp TEXT",
                 "data BLOB"});

    createIndexes("stashes", {"parent", "type", "realm", "league"});
}

void UserStore::loadLeagueList(const QString &realm)
{
    const QString stmt = "SELECT data FROM indexes WHERE ((name = 'leagues') AND (realm = ?))";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, realm);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load league index for realm='{}': {}", realm, message);
        return;
    }

    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read league index for realm='{}': {}", realm, message);
        return;
    }

    const QByteArray data = query.value(0).toByteArray();
    const auto [wrapper, ok] = json::parse_permissive<poe::LeagueListWrapper>(data);
    if (!ok) {
        spdlog::error("UserStore: error parsing league list");
        return;
    };
    auto leagues = std::make_shared<poe::LeagueList>(std::move(wrapper.leagues));
    emit leagueListReceived(leagues);
}

void UserStore::loadCharacterList(const QString &realm)
{
    const QString stmt = "SELECT data FROM indexes WHERE ((name = 'characters') AND (realm = ?))";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, realm);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load character index for {} realm: {}", realm, message);
        return;
    }

    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read character index for {} realm: {}", realm, message);
        return;
    }

    const QByteArray data = query.value(0).toByteArray();
    const auto [wrapper, ok] = json::parse_permissive<poe::CharacterListWrapper>(data);
    if (!ok) {
        spdlog::error("UserStore: error parsing character list");
        return;
    };
    auto characters = std::make_shared<poe::CharacterList>(std::move(wrapper.characters));
    emit characterListReceived(characters);
}

void UserStore::loadCharacters(const QString &realm, const QString &league)
{
    const QString stmt = "SELECT data FROM characters WHERE ((realm = ?) AND (league = ?))";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, realm);
    query.bindValue(1, league);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load characters for {} league in {} realm: {}",
                      league,
                      realm,
                      message);
        return;
    }

    while (query.next()) {
        const QByteArray data = query.value(0).toByteArray();
        const auto [wrapper, ok] = json::parse_permissive<poe::CharacterWrapper>(data);
        if (!ok) {
            spdlog::error("UserStore: error parsing character");
            return;
        }
        if (!wrapper.character) {
            spdlog::error("UserStore: character is empty");
            return;
        }
        auto character = std::make_shared<poe::Character>(std::move(*wrapper.character));
        emit characterReceived(character);
    }
}

void UserStore::loadStashList(const QString &realm, const QString &league)
{
    const QString stmt
        = "SELECT data FROM indexes WHERE ((name = 'stashes') AND (realm = ?) AND (league = ?))";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, realm);
    query.bindValue(1, league);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load stash index for {} league in {} realm: {}",
                      league,
                      realm,
                      message);
        return;
    }

    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read stash index for {} league in {} realm: {}",
                      league,
                      realm,
                      message);
        return;
    }

    const QByteArray data = query.value(0).toByteArray();
    const auto [wrapper, ok] = json::parse_permissive<poe::StashListWrapper>(data);
    if (!ok) {
        spdlog::error("UserStore: error parsing stash list");
        return;
    };
    auto stashes = std::make_shared<poe::StashList>(std::move(wrapper.stashes));
    emit stashListReceived(stashes);
}

void UserStore::loadStashes(const QString &realm, const QString &league)
{
    const QString stmt = "SELECT data FROM stashes WHERE ((realm = ?) AND (league = ?))";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, realm);
    query.bindValue(1, league);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load stashes for {} league in {} realm: {}",
                      league,
                      realm,
                      message);
        return;
    }

    while (query.next()) {
        const QByteArray data = query.value(0).toByteArray();
        const auto [wrapper, ok] = json::parse_permissive<poe::StashTabWrapper>(data);
        if (!ok) {
            spdlog::error("UserStore: error parsing stash");
            return;
        }
        if (!wrapper.stash) {
            spdlog::error("UserStore: stash is empty");
            return;
        }
        auto stash = std::make_shared<poe::StashTab>(std::move(*wrapper.stash));
        emit stashReceived(stash);
    }
}

void UserStore::handleLeagueList(QString realm, std::shared_ptr<QByteArray> data)
{
    if (!data) {
        spdlog::error("UserStore: league list data is null");
        return;
    }
    const auto [wrapper, ok] = json::parse_permissive<poe::LeagueListWrapper>(*data);
    if (!ok) {
        spdlog::error("UserStore: error parsing league list");
        return;
    };
    updateIndex("leagues", realm, "", *data);
    auto leagues = std::make_shared<poe::LeagueList>(std::move(wrapper.leagues));
    emit leagueListReceived(leagues);
}

void UserStore::handleCharacterList(QString realm, std::shared_ptr<QByteArray> data)
{
    if (!data) {
        spdlog::error("UserStore: character list data is null");
        return;
    }
    const auto [wrapper, ok] = json::parse_permissive<poe::CharacterListWrapper>(*data);
    if (!ok) {
        spdlog::error("UserStore: error parsing character list");
        return;
    }
    updateIndex("characters", realm, "", *data);
    auto characters = std::make_shared<poe::CharacterList>(wrapper.characters);
    emit characterListReceived(characters);
}

void UserStore::handleStashList(QString realm, QString league, std::shared_ptr<QByteArray> data)
{
    if (!data) {
        spdlog::error("UserStore: stash list data is null");
        return;
    }
    const auto [wrapper, ok] = json::parse_permissive<poe::StashListWrapper>(*data);
    if (!ok) {
        spdlog::error("UserStore: error parsing stash list");
        return;
    }
    updateIndex("stashes", realm, league, *data);
    auto stashes = std::make_shared<poe::StashList>(std::move(wrapper.stashes));
    emit stashListReceived(stashes);
};

void UserStore::handleCharacter(QString realm, QString name, std::shared_ptr<QByteArray> data)
{
    spdlog::trace("UserStore::handleCharacter(): realm='{}' name='{}'", realm, name);

    if (!data) {
        spdlog::error("UserStore: character data is null.");
    }

    const auto [wrapper, ok] = json::parse_permissive<poe::CharacterWrapper>(*data);
    if (!ok) {
        spdlog::error("UserStore: error parsing character.");
        return;
    }
    if (!wrapper.character) {
        spdlog::error("UserStore: character is missing.");
        return;
    }

    const poe::Character &character = *wrapper.character;

    if (realm != character.realm) {
        spdlog::warn("UserStore: using character realm '{}' but found '{}': {}",
                     realm,
                     character.realm,
                     *data);
    }
    if (name != character.name) {
        spdlog::warn("UserStore: using character name '{}' but found '{}'", name, character.name);
    }

    const QString stmt = "INSERT OR REPLACE INTO characters"
                         "(id, name, realm, league, timestamp, data)"
                         "VALUES (?, ?, ?, ?, ?, ?)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, character.id);
    query.bindValue(1, name);
    query.bindValue(2, realm);
    query.bindValue(3, character.league.value_or(""));
    query.bindValue(4, QDateTime::currentDateTime());
    query.bindValue(5, *data);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to update character '{}' in {} realm: {}",
                      name,
                      realm,
                      message);
    }
}

void UserStore::handleStash(QString realm,
                            QString league,
                            QString stash_id,
                            QString substash_id,
                            std::shared_ptr<QByteArray> data)
{
    spdlog::trace("UserStore::handleStash() realm='{}' league='{}' stash_id='{}' substash_id='{}'",
                  realm,
                  league,
                  stash_id,
                  substash_id);

    if (!data) {
        spdlog::error("UserStore: stash data is null.");
    }

    const auto [wrapper, ok] = json::parse_permissive<poe::StashTabWrapper>(*data);
    if (!ok) {
        spdlog::error("UserStore: error parsing stash.");
        return;
    }
    if (!wrapper.stash) {
        spdlog::error("UserStore: stash is missing");
        return;
    }

    const poe::StashTab &stash = *wrapper.stash;

    if (stash_id != stash.id) {
        spdlog::warn("UserStore: using stash id '{}' but found '{}'", stash_id, stash.id);
    }

    const QString stmt = "INSERT OR REPLACE INTO stashes"
                         "(id, parent, name, type, stash_index, realm, league, timestamp, data)"
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, stash_id);
    query.bindValue(1, stash.parent.value_or(""));
    query.bindValue(2, stash.name);
    query.bindValue(3, stash.type);
    if (stash.index) {
        query.bindValue(4, *stash.index);
    } else {
        query.bindValue(4, "");
    }
    query.bindValue(5, realm);
    query.bindValue(6, league);
    query.bindValue(7, QDateTime::currentDateTime());
    query.bindValue(8, *data);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to update stash for {} realm in {} league with "
                      "stash_id='{}' substash_id='{}': {}",
                      realm,
                      league,
                      stash_id,
                      substash_id,
                      message);
    }
}

void UserStore::updateIndex(const QString &name,
                            const QString &realm,
                            const QString &league,
                            const QByteArray &data)
{
    if (name.isEmpty()) {
        spdlog::error("UserStore: cannot update an index without a name.");
        return;
    }

    const QString stmt
        = "INSERT OR REPLACE INTO indexes (name, realm, league, timestamp, data) VALUES "
          "(?, ?, ?, ?, ?)";

    spdlog::trace("UserStore: updating {} index: '{}'", name, stmt);

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, name);
    query.bindValue(1, realm);
    query.bindValue(2, league);
    query.bindValue(3, QDateTime::currentDateTime());
    query.bindValue(4, data);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to update {} index: {}", name, message);
    }
}

QString UserStore::getPath(const QString &username)
{
    const QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    const QString filename = dir.absoluteFilePath(username + ".db");
    spdlog::trace("UserStore: file for '{}' is: '{}'", username, filename);
    return filename;
}
