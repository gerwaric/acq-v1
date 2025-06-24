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
    // List of available chararacters and stashes are stored here.
    createTable("indexes",
                {"name TEXT PRIMARY KEY",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp INTEGER",
                 "data TEXT"});

    // Full character data with items is stored here.
    createTable("characters",
                {"id TEXT PRIMARY KEY",
                 "name TEXT",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp INTEGER",
                 "data BLOB"});

    // Full stash data with items is stored here.
    createTable("stashes",
                {"id TEXT PRIMARY KEY",
                 "parent TEXT REFERENCES stashes(id)",
                 "name TEXT",
                 "type TEXT",
                 "stash_index INTEGER",
                 "realm TEXT",
                 "league TEXT",
                 "timestamp INTEGER",
                 "data BLOB"});

    createTable("buyouts",
                {"item_id TEXT",
                 "stash_id TEXT REFERENCES stashes(id)",
                 "note TEXT",
                 "source TEXT",
                 "timestamp INTEGER"});

    // Add indexes for the most common use cases.
    createIndexes("characters", {"realm", "league"});
    createIndexes("stashes", {"realm", "league", "parent", "type"});
    createIndexes("buyouts", {"item_id", "stash_id"});
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
    emit leagueListReceived(wrapper.leagues);
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
    emit characterListReceived(wrapper.characters);
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
        emit characterReceived(wrapper.character.value());
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
    emit stashListReceived(wrapper.stashes);
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
        emit stashReceived(wrapper.stash.value());
    }
}

void UserStore::saveLeagueList(const QString &realm,
                               const std::vector<poe::League> &leagues,
                               const QByteArray &data)
{
    updateIndex("leagues", realm, "", data);
    emit leagueListReceived(leagues);
}

void UserStore::saveCharacterList(const QString &realm,
                                  const std::vector<poe::Character> &characters,
                                  const QByteArray &data)
{
    updateIndex("characters", realm, "", data);
    emit characterListReceived(characters);
}

void UserStore::saveStashList(const QString &realm,
                              const QString &league,
                              const std::vector<poe::StashTab> &stashes,
                              const QByteArray &data)
{
    updateIndex("stashes", realm, league, data);
    emit stashListReceived(stashes);
};

void UserStore::saveCharacter(const QString &realm,
                              const QString &name,
                              const std::optional<poe::Character> &character,
                              const QByteArray &data)
{
    if (!character) {
        spdlog::error("UserStore: recieved empty character");
        return;
    }

    if (realm != character->realm) {
        spdlog::warn("UserStore: using character realm '{}' but found '{}': {}",
                     realm,
                     character->realm,
                     data);
    }
    if (name != character->name) {
        spdlog::warn("UserStore: using character name '{}' but found '{}'", name, character->name);
    }

    const QString stmt = "INSERT OR REPLACE INTO characters"
                         "(id, name, realm, league, timestamp, data)"
                         "VALUES (?, ?, ?, ?, ?, ?)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, character->id);
    query.bindValue(1, name);
    query.bindValue(2, realm);
    query.bindValue(3, character->league.value_or(""));
    query.bindValue(4, QDateTime::currentMSecsSinceEpoch());
    query.bindValue(5, data);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to update character '{}' in {} realm: {}",
                      name,
                      realm,
                      message);
    }
}

void UserStore::saveStash(const QString &realm,
                          const QString &league,
                          const QString &stash_id,
                          const QString &substash_id,
                          const std::optional<poe::StashTab> &stash,
                          const QByteArray &data)
{
    if (!stash) {
        spdlog::error("UserStore: recieved empty stash");
        return;
    }

    const QString id = substash_id.isEmpty() ? stash_id : substash_id;
    const QString parent_id = substash_id.isEmpty() ? "" : stash_id;

    if (stash->id != id) {
        spdlog::warn("UserStore: expected stash '{}' but received '{}'", id, stash->id);
    }

    if (stash->parent) {
        if (stash->parent.value() != id) {
            spdlog::warn("UserStore: expected stash parent '{}' but received '{}'",
                         parent_id,
                         stash->parent.value());
        }
    }

    const QString stmt = "INSERT OR REPLACE INTO stashes"
                         "(id, parent, name, type, stash_index, realm, league, timestamp, data)"
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);
    query.bindValue(0, stash->id);
    query.bindValue(1, stash->parent.value_or(""));
    query.bindValue(2, stash->name);
    query.bindValue(3, stash->type);
    if (stash->index) {
        query.bindValue(4, stash->index.value());
    } else {
        query.bindValue(4, "");
    }
    query.bindValue(5, realm);
    query.bindValue(6, league);
    query.bindValue(7, QDateTime::currentMSecsSinceEpoch());
    query.bindValue(8, data);

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
    query.bindValue(3, QDateTime::currentMSecsSinceEpoch());
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
