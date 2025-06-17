// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "app.h"

#include "util/json.h"
#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

App::App(QObject *parent)
    : QObject(parent)
    , m_oauthManager(m_networkManager)
    , m_rateLimiter(m_networkManager)
{
    const auto [token, ok] = m_globalData.retrieve<OAuthToken>("oauth_token");
    if (ok) {
        authenticated(token);
    };

    connect(&m_oauthManager, &OAuthManager::accessGranted, this, &App::authenticated);
}

void App::authenticate()
{
    m_oauthManager.initLogin();
}

void App::authenticated(const OAuthToken &token)
{
    // First store the token.
    const QString json_token = json::toString(token);
    m_globalData.set("oauth_token", json_token);

    // Then update ourselves.
    m_networkManager.setBearerToken(token.access_token);
    m_authenticated = true;
    m_username = token.username;

    // Finally notify anyone listening.
    if ((!m_authenticated) || (m_username != token.username)) {
        emit authenticationStateChanged();
    };
}
