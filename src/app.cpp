// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "util/json.h"
#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

App::App(QObject *parent)
    : QObject(parent)
    , m_oauthManager(m_networkManager)
    , m_rateLimiter(m_networkManager)
{
    connect(&m_oauthManager, &OAuthManager::grantAccess, this, &App::accessGranted);
    connect(&m_poeClient, &PoeClient::requestReady, &m_rateLimiter, &RateLimiter::makeRequest);
    connect(&m_rateLimiter, &RateLimiter::Paused, this, &App::rateLimited);

    const auto [token, ok] = m_globalStore.retrieve<OAuthToken>("oauth_token");
    if (ok) {
        m_oauthManager.setToken(token);
    }

    spdlog::info("App: creating items store");
}

void App::authenticate()
{
    m_oauthManager.initLogin();
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
    // TODO: Special cases for map tabs and unique tab.

    for (const auto &stash : m_stashList) {
        spdlog::info("App: requesting stash '{}' ({})", stash.name, stash.id);
        m_poeClient.getStash(m_realm, m_league, stash.id, "");
        if (stash.children) {
            for (const auto &child : stash.children.value()) {
                spdlog::info("App: requesting stash '{}' ({}): child of '{}' (){})",
                             child.name,
                             child.id,
                             stash.name,
                             stash.id);

                // A child should have either a parent or a folder, but not both.
                if (child.parent && child.folder) {
                    // The child appears to have both.
                    spdlog::warn("App: child stash '{}' ({}) has both parent ({}) and folder ({}).",
                                 child.name,
                                 child.id,
                                 child.parent.value_or(""),
                                 child.folder.value_or(""));
                } else if ((!child.parent) && (!child.folder)) {
                    // The child appears to have neither.
                    spdlog::warn("App: child stash '{}' ({}) has neither parent nor folder",
                                 child.name,
                                 child.id);
                }

                // We only use substash id for children with a parent.
                if (child.parent) {
                    m_poeClient.getStash(m_realm, m_league, stash.id, child.id);
                } else {
                    m_poeClient.getStash(m_realm, m_league, child.id, "");
                }

                // We don't support grandchildren right now.
                if (child.children) {
                    spdlog::error("App: stash '{}' ({}) child '{}' ({}) also has children.",
                                  stash.name,
                                  stash.id,
                                  child.name,
                                  child.id);
                }
            }
        }
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
        if (stash.children) {
            for (const auto &child : stash.children.value()) {
                names.append(stash.name + " / " + child.name);
                if (child.children) {
                    spdlog::error("App: stash child has children");
                }
            }
        }
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

    connect(&m_poeClient, &PoeClient::leagueListReceived, m_userStore, &UserStore::saveLeagueList);
    connect(&m_poeClient,
            &PoeClient::characterListReceived,
            m_userStore,
            &UserStore::saveCharacterList);
    connect(&m_poeClient, &PoeClient::stashListReceived, m_userStore, &UserStore::saveStashList);
    connect(&m_poeClient, &PoeClient::characterReceived, m_userStore, &UserStore::saveCharacter);
    connect(&m_poeClient, &PoeClient::stashReceived, m_userStore, &UserStore::saveStash);

    connect(&m_poeClient, &PoeClient::leagueListReceived, this, &App::handleLeagueList);
    connect(&m_poeClient, &PoeClient::characterListReceived, this, &App::handleCharacterList);
    connect(&m_poeClient, &PoeClient::characterReceived, this, &App::handleCharacter);
    connect(&m_poeClient, &PoeClient::stashListReceived, this, &App::handleStashList);
    connect(&m_poeClient, &PoeClient::stashReceived, this, &App::handleStash);

    m_userStore->loadCharacterList(m_realm);
    m_userStore->loadLeagueList(m_realm);
    m_userStore->loadStashList(m_realm, m_league);
    m_userStore->loadStashes(m_realm, m_league);

    emit authenticationStateChanged();
}

void App::rateLimited(int seconds, const QString &policy_name)
{
    m_rateLimitStatus = (seconds <= 0) ? "Not rate limited."
                                       : QString("Rate limited for %1 seconds by %2 policy.")
                                             .arg(QString::number(seconds), policy_name);
    emit rateLimitStatusChanged();
}

void App::handleLeagueList(const QString &realm,
                           const std::vector<poe::League> &leagues,
                           const QByteArray &data)
{
    m_leagueList = leagues;
    emit leaguesUpdated();
}

void App::handleCharacterList(const QString &realm,
                              const std::vector<poe::Character> &characters,
                              const QByteArray &data)
{
    m_characterList = characters;
    emit charactersUpdated();
}

void App::handleCharacter(const QString &realm,
                          const QString &name,
                          const std::optional<poe::Character> &character,
                          const QByteArray &data)
{
    if (character) {
        m_model.appendCharacter(character.value());
    } else {
        spdlog::error("App: character is empty: realm({}) name({})", realm, name);
    }
}

void App::handleStash(const QString &realm,
                      const QString &league,
                      const QString &stash_id,
                      const QString &substash_id,
                      const std::optional<poe::StashTab> &stash,
                      const QByteArray &data)
{
    if (stash) {
        m_model.appendStash(stash.value());
    } else {
        spdlog::error("App: stash is empty: realm({}) league({}) stash_id({}) substash_id({})",
                      realm,
                      league,
                      stash_id,
                      substash_id);
    }
}

void App::handleStashList(const QString &realm,
                          const QString &league,
                          const std::vector<poe::StashTab> &stashes,
                          const QByteArray &data)
{
    spdlog::info("App: received a list of stashes with {} items.", stashes.size());
    m_stashList = stashes;
    emit stashesUpdated();
}

void App::loadSelectedCharacters()
{
    m_userStore->loadCharacters(m_realm, m_league);
}

void App::loadSelectedStashes()
{
    m_userStore->loadStashes(m_realm, m_league);
}
