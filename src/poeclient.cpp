// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "poeclient.h"

#include "poe.h"
#include "util/json.h"
#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

namespace {

    // This is the base for all API calls.
    constexpr const char *API_ENDPOINT = "https://api.pathofexile.com";

    // Hard-code the enpoint names because these are used by the rate limiter.
    constexpr const char *LIST_ACCOUNT_LEAGUES = "LIST_ACCOUNT_LEAGES";
    constexpr const char *LIST_CHARACTERS = "LIST_CHARACTERS";
    constexpr const char *LIST_STASHES = "LIST_STASHES";
    constexpr const char *GET_CHARACTER = "GET_CHARACTER";
    constexpr const char *GET_STASH = "GET_STASH";

    // Only some endpoint support poe2 right now.
    constexpr std::array<std::pair<const char *, bool>, 5> ENABLE_POE2_BY_ENDPOINT = {
        {{LIST_ACCOUNT_LEAGUES, false},
         {LIST_CHARACTERS, true},
         {LIST_STASHES, false},
         {GET_CHARACTER, false},
         {GET_STASH, false}}};

    // Create a function to lookup poe2 support.
    constexpr bool USE_POE2(const char *endpoint)
    {
        for (const auto &[key, value] : ENABLE_POE2_BY_ENDPOINT) {
            if (key == endpoint) {
                return value;
            }
        }
        return false;
    }

    poe::LeagueListPtr unwrapLeagueList(const QByteArray &data)
    {
        auto x = json::parse_strict<poe::StashTab>(data);
        /*
        poe::LeagueListWrapper wrapper;
        const std::string buffer(data.constData(), data.size());
        auto ctx = glz::context();
        auto pe = glz::read<glz::opts{}>(wrapper, buffer, ctx);
        if (pe) {
            std::string descriptive_error = glz::format_error(pe, buffer);
        }
        glz::read_json();
        const auto [wrapper, ok] = json::from_json<poe::LeagueListWrapper>(data, json::PERMISSIVE);
        if (!ok) {
            spdlog::error("PoE: error unwrapping stash list.");
            return nullptr;
        };
        return std::make_shared<poe::LeagueList>(wrapper.leagues);
        */
        return nullptr;
    }

} // namespace

PoeClient::PoeClient(QObject *parent)
    : QObject(parent) {};

// https://www.pathofexile.com/developer/docs/reference#leagues-list
void PoeClient::listLeagues(const QString &realm)
{
    spdlog::debug("PoE: list account leagues: realm='{}'", realm);

    // Check the realm.
    constexpr const char *endpoint = LIST_ACCOUNT_LEAGUES;
    if (!checkRealm(realm, endpoint)) {
        return;
    }

    // Build the request.
    QStringList parts = {API_ENDPOINT, "account", "leagues"};
    if (!realm.isEmpty() && (realm != "pc")) {
        parts.append(realm);
    }

    // Send the request.
    const QUrl url(parts.join("/"));
    emit sendRequest(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: emitting leagueListRecieved.");
        emit leagueListReceived(realm, getData(reply));
    });
}

// https://www.pathofexile.com/developer/docs/reference#characters-list
void PoeClient::listCharacters(const QString &realm)
{
    spdlog::debug("PoE: list characters: realm='{}'", realm);

    // Check the realm.
    constexpr const char *endpoint = LIST_CHARACTERS;
    if (!checkRealm(realm, endpoint)) {
        return;
    }

    // Build the request.
    QStringList parts = {API_ENDPOINT, "character"};
    if (!realm.isEmpty() && (realm != "pc")) {
        parts.append(realm);
    }

    // Send the request.
    const QUrl url(parts.join("/"));
    emit sendRequest(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: emitting characterListReceived.");
        emit characterListReceived(realm, getData(reply));
    });
}

// https://www.pathofexile.com/developer/docs/reference#stashes-list
void PoeClient::listStashes(const QString &realm, const QString &league)
{
    spdlog::debug("PoE: list stashes: ream='{}', league='{}'", realm, league);

    // Check the realm.
    constexpr const char *endpoint = LIST_STASHES;
    if (!checkRealm(realm, endpoint)) {
        return;
    }
    if (league.isEmpty()) {
        spdlog::error("PoE: cannot list stashes: league is empty");
        return;
    }

    // Build the request.
    QStringList parts = {API_ENDPOINT, "stash"};
    if (!realm.isEmpty() && (realm != "pc")) {
        parts.append(realm);
    }
    parts.append(league);

    // Send the request.
    const QUrl url(parts.join("/"));
    emit sendRequest(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: emitting stashListReceived.");
        emit stashListReceived(realm, league, getData(reply));
    });
}

// https://www.pathofexile.com/developer/docs/reference#characters-get
void PoeClient::getCharacter(const QString &realm, const QString &name)
{
    spdlog::debug("PoE: get character: realm='{}', name='{}'", realm, name);

    // Check the realm.
    constexpr const char *endpoint = GET_CHARACTER;
    if (!checkRealm(realm, endpoint)) {
        return;
    }
    if (name.isEmpty()) {
        spdlog::error("PoE: cannot get character: name is empty.");
        return;
    }

    // Build the request.
    QStringList parts = {API_ENDPOINT, "character"};
    if (!realm.isEmpty() && (realm != "pc")) {
        parts.append(realm);
    }
    parts.append(name);

    // Send the request.
    const QUrl url(parts.join("/"));
    emit sendRequest(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: emitting characterReceived.");
        emit characterReceived(realm, name, getData(reply));
    });
}

// https://www.pathofexile.com/developer/docs/reference#stashes-get
void PoeClient::getStash(const QString &realm,
                         const QString &league,
                         const QString &stash_id,
                         const QString &substash_id)
{
    spdlog::debug("PoE: get stash: realm='{}', league='{}', stash_id='{}', substash_id='{}'",
                  realm,
                  league,
                  stash_id,
                  substash_id);

    // Check the realm.
    constexpr const char *endpoint = GET_STASH;
    if (!checkRealm(realm, endpoint)) {
        return;
    }
    if (league.isEmpty()) {
        spdlog::error("PoE: cannot list stashes: league is empty.");
        return;
    }
    if (stash_id.isEmpty()) {
        spdlog::error("PoE: cannot get stash: stash_id is empty.");
        return;
    }

    // Build the request.
    QStringList parts = {API_ENDPOINT, "stash"};
    if (!realm.isEmpty() && (realm != "pc")) {
        parts.append(realm);
    }
    parts.append(league);
    parts.append(stash_id);
    if (!substash_id.isEmpty()) {
        parts.append(substash_id);
    }

    // Send the request.
    const QUrl url(parts.join("/"));
    emit sendRequest(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: emitting stashReceived.");
        emit stashReceived(realm, league, stash_id, substash_id, getData(reply));
    });
}

bool PoeClient::checkRealm(const QString &realm, const char *endpoint)
{
    const bool poe1 = (realm == "") || (realm == "pc");
    const bool poe2 = (realm == "poe2") && USE_POE2(endpoint);
    const bool ok = poe1 || (realm == "xbox") || (realm == "sony") || poe2;
    if (!ok) {
        spdlog::error("PoE: invalid realm for {}: {}", endpoint, realm);
    }
    return ok;
}

std::shared_ptr<QByteArray> PoeClient::getData(QNetworkReply *reply)
{
    if (!reply) {
        spdlog::error("PoE: reply was null.");
        return nullptr;
    }
    reply->deleteLater();
    return std::make_shared<QByteArray>(reply->readAll());
}
