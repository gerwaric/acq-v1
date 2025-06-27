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

// Cannot declare these structures in an anonymous namespace
// because glaze needs to use them.

struct LeagueListWrapper
{
    std::vector<poe::League> leagues;
};

struct CharacterListWrapper
{
    std::vector<poe::Character> characters;
};

struct StashListWrapper
{
    std::vector<poe::StashTab> stashes;
};

struct CharacterWrapper
{
    std::optional<poe::Character> character;
};

struct StashTabWrapper
{
    std::optional<poe::StashTab> stash;
};

UserStore::UserStore(const QString &username, QObject *parent)
    : DataStore(getPath(username), parent)
{
    // List of available chararacters and stashes are stored here.
    createTable("indexes",
                {"name TEXT", "realm TEXT", "league TEXT", "timestamp INTEGER", "data TEXT"});

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

    // Setup the sql models.
    auto db = getThreadLocalDatabase();
    m_characterModel.setQuery("SELECT name, realm, league, timestamp FROM characters", db);
    m_stashModel.setQuery("SELECT name, type, league, id, parent, timestamp from stashes", db);
}

QStringList UserStore::getLeagueNames(const QString &realm)
{
    const QString statement{"SELECT league FROM characters WHERE realm = :realm"
                            " UNION"
                            " SELECT league FROM stashes WHERE realm = :realm"
                            " ORDER BY league"};

    // Build the query.
    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);

    // Run the query.
    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to get leagues realm='{}': {}", realm, message);
        return {};
    }

    // Return the results.
    QStringList names;
    while (query.next()) {
        names.append(query.value(0).toString());
    }
    return names;
}

std::vector<poe::League> UserStore::getLeagueList(const QString &realm)
{
    const QByteArray data = getIndex("leagues", realm, "");
    auto [wrapper, ok] = json::parse_permissive<poe::LeagueListWrapper>(data);
    if (ok) {
        return wrapper.leagues;
    } else {
        spdlog::error("UserStore: error parsing leagues index");
        return {};
    }
}

std::vector<poe::Character> UserStore::getCharacterList(const QString &realm)
{
    const QByteArray data = getIndex("characters", realm, "");
    const std::string_view json(data.constData(), data.size());

    poe::CharacterListWrapper wrapper;
    glz::context ctx{};
    const auto opts = glz::opts{.error_on_unknown_keys = false};
    const auto err = glz::read<opts>(wrapper, json, ctx);
    if (err) {
        const std::string type_name = typeid(T).name();
        const std::string error_message = glz::format_error(err, json);
        spdlog::error("json error parsing {}: {}", type_name, error_message);
    }

    auto [wrapper, ok] = json::parse_permissive<poe::CharacterListWrapper>(data);
    if (ok) {
        return wrapper.characters;
    } else {
        spdlog::error("UserStore: error parsing character index");
        return {};
    }
}

std::vector<poe::StashTab> UserStore::getStashList(const QString &realm, const QString &league)
{
    const QByteArray data = getIndex("stashes", realm, league);
    auto [wrapper, ok] = json::parse_permissive<poe::StashListWrapper>(data);
    if (ok) {
        return wrapper.stashes;
    } else {
        spdlog::error("UserStore: error parsing stash index");
        return {};
    }
}

std::optional<poe::Character> UserStore::getCharacter(const QString &realm, const QString &name)
{
    const QString statement{"SELECT data FROM characters WHERE realm = :realm AND name = :name"};

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);
    query.bindValue(":name", name);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load league index for realm='{}': {}", realm, message);
        return {};
    }

    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read league index for realm='{}': {}", realm, message);
        return {};
    }

    const QByteArray data = query.value(0).toByteArray();
    const auto [wrapper, ok] = json::parse_permissive<CharacterWrapper>(data);
    if (!ok) {
        spdlog::error("UserStore: error parsing character wrapper");
        return {};
    }
    return wrapper.character;
}

std::optional<poe::StashTab> UserStore::getStash(const QString &realm,
                                                 const QString &league,
                                                 const QString &id)
{
    const QString statement{"SELECT data FROM stashes WHERE realm = :realm"
                            " AND league = :league"
                            " AND id = :id"};

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);
    query.bindValue(":id", id);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to load league index for realm='{}': {}", realm, message);
        return {};
    }

    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read league index for realm='{}': {}", realm, message);
        return {};
    }

    const QByteArray data = query.value(0).toByteArray();
    const auto [wrapper, ok] = json::parse_permissive<poe::StashWrapper>(data);
    if (!ok) {
        spdlog::error("UserStore: error parsing character wrapper");
        return {};
    }
    return wrapper.stash;
}

void UserStore::loadLeagueList(const QString &realm)
{
    const QString statement{"SELECT data FROM indexes WHERE name = 'leagues' AND realm = :realm"};

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);

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
    emit leagueListReady(wrapper.leagues);
}

void UserStore::loadCharacterList(const QString &realm)
{
    const QString statement{"SELECT data FROM indexes"
                            " WHERE name = 'characters' AND realm = :realm"};

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);

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
    emit characterListReady(wrapper.characters);
}

void UserStore::loadCharacters(const QString &realm, const QString &league)
{
    const QString statement{"SELECT data FROM characters"
                            " WHERE realm = :realm AND league = :league"};

    // Build the query.
    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);

    // Run the query.
    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to get characters in {}/{}: {}", realm, league, message);
        return;
    }

    // Parse and emit the results.
    while (query.next()) {
        const QString data = query.value(0).toByteArray();
        const auto [wrapper, ok] = json::parse_permissive<poe::CharacterWrapper>(data);
        if (!ok) {
            spdlog::error("UserData: error parsing character");
        } else if (!wrapper.character) {
            spdlog::error("UserData: character wrapper is empty");
        } else {
            emit characterReady(wrapper.character.value());
        }
    }
}

void UserStore::loadStashList(const QString &realm, const QString &league)
{
    const QString statement{"SELECT data FROM indexes"
                            " WHERE name = 'stashes' AND realm = :realm AND league = :league"};

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);

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
    emit stashListReady(realm, league, wrapper.stashes);
}

void UserStore::loadStashes(const QString &realm, const QString &league)
{
    const QString statement{"SELECT data FROM stashes WHERE"
                            " realm = :realm AND"
                            " league = :league"};

    // Build the query.
    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);

    // Run the query.
    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to get stashes in {}/{}: {}", realm, league, message);
        return;
    }

    // Parse and emit the results.
    while (query.next()) {
        const QString data = query.value(0).toByteArray();
        const auto [wrapper, ok] = json::parse_permissive<StashTabWrapper>(data);
        if (!ok) {
            spdlog::error("UserStore: error parsing stash tab");
        } else if (!wrapper.stash) {
            spdlog::error("UserData: stash wrapper is empty");
        } else {
            emit stashReady(realm, league, wrapper.stash.value());
        }
    }
}

void UserStore::storeLeagueListData(const QString &realm, const QByteArray &data)
{
    const auto [wrapper, ok] = json::parse_permissive<poe::LeagueListWrapper>(data);
    if (ok) {
        updateIndex("leagues", realm, "", data);
        emit leagueListReady(wrapper.leagues);
    } else {
        spdlog::error("UserStore: error parsing league list wrapper.");
    }
}

void UserStore::storeCharacterListData(const QString &realm, const QByteArray &data)
{
    const auto [wrapper, ok] = json::parse_permissive<poe::CharacterListWrapper>(data);
    if (ok) {
        updateIndex("characters", realm, "", data);
        emit characterListReady(wrapper.characters);
    } else {
        spdlog::error("UserStore: error parsing character list wrapper.");
    }
}

void UserStore::storeStashListData(const QString &realm,
                                   const QString &league,
                                   const QByteArray &data)
{
    const auto [wrapper, ok] = json::parse_permissive<poe::StashListWrapper>(data);
    if (ok) {
        updateIndex("stashes", realm, league, data);
        emit stashListReady(realm, league, wrapper.stashes);
    } else {
        spdlog::error("UserStore: error parsing stash list wrapper.");
    }
};

void UserStore::storeCharacterData(const QString &realm, const QString &name, const QByteArray &data)
{
    const auto [wrapper, ok] = json::parse_permissive<poe::CharacterWrapper>(data);
    if (!wrapper.character) {
        spdlog::error("UserStore: recieved empty character");
        return;
    }
    const auto character = wrapper.character.value();

    if (realm != character.realm) {
        spdlog::warn("UserStore: using character realm '{}' but found '{}': {}",
                     realm,
                     character.realm,
                     data);
    }
    if (name != character.name) {
        spdlog::warn("UserStore: using character name '{}' but found '{}'", name, character.name);
    }

    const QString statement = "INSERT OR REPLACE INTO characters"
                              " (id, name, realm, league, timestamp, data)"
                              " VALUES"
                              " (:id, :name, :realm, :league, :timestamp, :data)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":id", character.id);
    query.bindValue(":name", name);
    query.bindValue(":realm", realm);
    query.bindValue(":league", character.league.value_or(""));
    query.bindValue(":timestampe", QDateTime::currentMSecsSinceEpoch());
    query.bindValue(":data", data);

    if (query.exec()) {
        emit characterReady(character);
    } else {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to update character '{}' in {} realm: {}",
                      name,
                      realm,
                      message);
    }
}

void UserStore::storeStashData(const QString &realm,
                               const QString &league,
                               const QString &stash_id,
                               const QString &substash_id,
                               const QByteArray &data)
{
    const auto [wrapper, ok] = json::parse_permissive<StashTabWrapper>(data);
    if (!wrapper.stash) {
        spdlog::error("UserStore: recieved empty stash");
        return;
    }

    const auto &stash = wrapper.stash.value();
    const QString id = substash_id.isEmpty() ? stash_id : substash_id;
    const QString parent_id = substash_id.isEmpty() ? "" : stash_id;

    if (stash.id != id) {
        spdlog::warn("UserStore: expected stash '{}' but received '{}'", id, stash.id);
    }

    if (stash.parent) {
        if (stash.parent.value() != id) {
            spdlog::warn("UserStore: expected stash parent '{}' but received '{}'",
                         parent_id,
                         stash.parent.value());
        }
    }

    const QString statement
        = "INSERT OR REPLACE INTO stashes"
          " (id, parent, name, type, stash_index, realm, league, timestamp, data)"
          " VALUES"
          " (:id, :parent, :name, :type, :stash_index, :realm, :league, :timestampe, :data)";

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":id", stash.id);
    query.bindValue(":parent", stash.parent.value_or(""));
    query.bindValue(":name", stash.name);
    query.bindValue(":type", stash.type);
    if (stash.index) {
        query.bindValue(":stash_index", stash.index.value());
    } else {
        query.bindValue(":stash_index", "");
    }
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);
    query.bindValue(":timestamp", QDateTime::currentMSecsSinceEpoch());
    query.bindValue(":data", data);

    if (query.exec()) {
        emit stashReady(realm, league, stash);
    } else {
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

QByteArray UserStore::getIndex(const QString &name, const QString &realm, const QString &league)
{
    const QString statement{"SELECT data FROM indexes WHERE"
                            " name = :name AND"
                            " realm = :realm AND"
                            " league = :league"};

    spdlog::info("GET_INDEX: {}, {}, {}", name, realm, league);

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":name", name);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to get {} index: {}", name, message);
        return {};
    }
    if (!query.next()) {
        const QString message = query.lastError().text();
        spdlog::error("UserStore: failed to read {} index: {}", name, message);
        return {};
    }
    return query.value(0).toByteArray();
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

    const QString statement{"INSERT OR REPLACE INTO indexes"
                            " (name, realm, league, timestamp, data)"
                            " VALUES"
                            " (:name, :realm, :league, :timestamp, :data)"};

    spdlog::trace("UserStore: updating {} index: '{}'", name, statement);

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(statement);
    query.bindValue(":name", name);
    query.bindValue(":realm", realm);
    query.bindValue(":league", league);
    query.bindValue(":timestamp", QDateTime::currentMSecsSinceEpoch());
    query.bindValue(":data", data);

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
