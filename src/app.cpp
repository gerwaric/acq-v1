// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "util/json.h"
#include "util/qt.h"
#include "util/spdlog_qt.h"

#include <QSqlDatabase>

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

App::App(QObject *parent)
    : QObject{parent}
    , m_oauthManager{m_networkManager}
    , m_rateLimiter{m_networkManager}
    , m_itemSelectionModel{&m_itemModel}
{
    connect(&m_oauthManager, &OAuthManager::grantAccess, this, &App::accessGranted);
    connect(&m_rateLimiter, &RateLimiter::Paused, this, &App::rateLimited);
    connect(&m_client, &PoeClient::requestReady, &m_rateLimiter, &RateLimiter::makeRequest);
    connect(&m_itemSelectionModel,
            &QItemSelectionModel::currentChanged,
            this,
            &App::selectionChanged);

    // Look for an existing username.
    const QString username = m_globalStore.get("last_username").toString();
    if (!username.isEmpty()) {
        spdlog::info("App: username is '{}'", username);
        m_username = username;
        m_clientStore = std::make_unique<UserStore>(username);
        m_clientStore->connectTo(m_client);
    }

    // Look for an existing OAuth token.
    const auto [token, ok] = m_globalStore.retrieve<OAuthToken>("oauth_token");
    if (ok) {
        spdlog::debug("App: found an existing token {}", token.refresh_token);
        m_oauthManager.setToken(token);
    }
}

QString App::getLogLevel() const
{
    const auto level = spdlog::get_level();
    const auto name = spdlog::level::to_string_view(level);
    return QString::fromUtf8(name);
}

void App::setLogLevel(const QString &level)
{
    const auto str = level.toUtf8().toStdString();
    const auto lvl = spdlog::level::from_str(str);
    spdlog::set_level(lvl);
    spdlog::info("App: logging level set to {}", spdlog::get_level());
};

void App::authenticate()
{
    m_oauthManager.initLogin();
}

QStringList App::getRealmNames() const
{
    return util::toStringList<PoeClient::Realm>();
}

QStringList App::getLeagueNames(const QString &realm) const
{
    if (!m_clientStore) {
        spdlog::error("App: cannot get league names: repo is uninitialized.");
        return {};
    }

    // Get the league list.
    const auto leagues = m_clientStore->getLeagueList(realm);

    // Build a list of names.
    QStringList names;
    names.reserve(leagues.size());
    for (const auto &league : leagues) {
        names.append(league.name);
    }
    return names;
}

void App::loadItems(const QString &realm, const QString &league)
{
    if (!m_clientStore) {
        spdlog::error("App: cannot load items: repo is uninitialized.");
        return;
    }
    m_clientStore->loadCharacters(realm, league);
    m_clientStore->loadStashes(realm, league);
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
                names.append(">> " + child.name);
                if (child.children) {
                    spdlog::error("App: stash child has children");
                }
            }
        }
    }
    return names;
}

void App::getCharacter()
{

}

void App::getAllCharacters()
{
}

void App::getAllStashes()
{
    // TODO: Special cases for map tabs and unique tab.

    if (!m_clientStore) {
        spdlog::warn("App: cannot get stashes: repo is uninitialized.");
        return;
    }

    for (const auto &stash : m_stashList) {
        spdlog::info("App: requesting stash '{}' ({})", stash.name, stash.id);
        m_client.getStash(m_realm, m_league, stash.id, "");
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
                    m_client.getStash(m_realm, m_league, stash.id, child.id);
                } else {
                    m_client.getStash(m_realm, m_league, child.id, "");
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

void App::accessGranted(const OAuthToken &token)
{
    // Store the token and username
    m_globalStore.set("oauth_token", json::toString(token));
    m_globalStore.set("last_username", token.username);

    // Update ourselves.
    m_authenticated = true;
    m_username = token.username;
    m_networkManager.setBearerToken(token.access_token);
    m_clientStore = std::make_unique<UserStore>(m_username);
    m_clientStore->connectTo(m_client);

    auto *client = m_clientStore.get();
    connect(client, &UserStore::characterReady, &m_itemModel, &TreeModel::addCharacter);
    connect(client, &UserStore::stashReady, &m_itemModel, &TreeModel::addStash);

    QSqlDatabase db = m_clientStore->getDatabase();
    m_characterTableModel.setQuery("SELECT name, realm, league, timestamp FROM characters", db);
    m_stashTableModel.setQuery("SELECT name, type, league, id, parent, timestamp from stashes", db);

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

void App::loadSelectedCharacters()
{
    //m_userStore->loadCharacters(m_realm, m_league);
}

void App::loadSelectedStashes()
{
    //m_userStore->loadStashes(m_realm, m_league);
}

void App::selectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid()) {
        spdlog::warn("App: the seelction changed but the current index is invalid.");
        return;
    }

    const auto node = m_itemModel.getNode(current);

    if (!node) {
        spdlog::warn("App: the selection changed but the node is invalid.");
        return;
    }

    if (node->isItem()) {
        const auto &payload = node->payload();
        const auto &item_info = std::get<ItemInfo>(payload);
        m_tooltip = std::make_unique<ItemTooltip>(item_info);
        emit tooltipChanged();
    }
}
