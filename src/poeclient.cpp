// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "poeclient.h"

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

} // namespace

PoeClient::PoeClient(QObject *parent)
    : QObject(parent) {};

// https://www.pathofexile.com/developer/docs/reference#leagues-list
void PoeClient::listLeagues(const QString &realm)
{
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
    const QString tag = parts.mid(1).join("/");
    const QUrl url(parts.join("/"));
    spdlog::info("PoE: requesting list: {}", tag);

    emit requestReady(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::info("PoE: received list: {}", tag);
        emit leagueListDataReceived(realm, reply->readAll());
        reply->deleteLater();
    });
}

// https://www.pathofexile.com/developer/docs/reference#characters-list
void PoeClient::listCharacters(const QString &realm)
{
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
    const QString tag = parts.mid(1).join("/");
    const QUrl url(parts.join("/"));
    spdlog::info("PoE: requesting list: {}", tag);

    emit requestReady(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: received list: {}", tag);
        emit characterListDataReceived(realm, reply->readAll());
        reply->deleteLater();
    });
}

// https://www.pathofexile.com/developer/docs/reference#stashes-list
void PoeClient::listStashes(const QString &realm, const QString &league)
{
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
    const QString tag = parts.mid(1).join("/");
    const QUrl url(parts.join("/"));
    spdlog::info("PoE: requesting list: {}", tag);

    emit requestReady(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        spdlog::trace("PoE: received list: {}", tag);
        emit stashListDataReceived(realm, league, reply->readAll());
        reply->deleteLater();
    });
}

// https://www.pathofexile.com/developer/docs/reference#characters-get
void PoeClient::getCharacter(const QString &realm, const QString &name)
{
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
    const QString tag = parts.mid(1).join("/");
    const QUrl url(parts.join("/"));
    spdlog::info("PoE: requesting {} ({} pending).", tag, m_pendingCharacterRequests);
    ++m_pendingCharacterRequests;

    emit requestReady(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        --m_pendingCharacterRequests;
        spdlog::info("PoE: received {} ({} pending).", tag, m_pendingCharacterRequests);
        emit characterDataReceived(realm, name, reply->readAll());
        reply->deleteLater();
    });
}

// https://www.pathofexile.com/developer/docs/reference#stashes-get
void PoeClient::getStash(const QString &realm,
                         const QString &league,
                         const QString &stash_id,
                         const QString &substash_id)
{
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
    const QString tag = parts.mid(1).join("/");
    const QUrl url(parts.join("/"));
    spdlog::info("PoE: requesting {} ({} pending)", tag, m_pendingStashRequests);
    ++m_pendingStashRequests;

    emit requestReady(endpoint, QNetworkRequest(url), [=, this](QNetworkReply *reply) {
        --m_pendingStashRequests;
        spdlog::info("PoE: received {} ({} pending).", tag, m_pendingStashRequests);
        emit stashDataReceived(realm, league, stash_id, substash_id, reply->readAll());
        reply->deleteLater();
    });
}

std::vector<poe::League> unwrapLeageList(const QByteArray &data)
{
    return {};
}

std::vector<poe::Character> unwrapCharacterList(const QByteArray &data)
{
    return {};
}

std::vector<poe::StashTab> unwrapStashList(const QByteArray &data)
{
    return {};
}

std::optional<poe::Character> unwrapCharacter(const QByteArray &data)
{
    return {};
}

std::optional<poe::StashTab> unwrapStash(const QByteArray &data)
{
    return {};
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
