// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "oauthmanager.h"

#include "networkmanager.h"
#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

#include <QByteArray>
#include <QDesktopServices>
#include <QSet>

#include <array>

namespace {

    // Hard-coded settings for acquisition.
    constexpr const char *AUTHORIZATION_URL = "https://www.pathofexile.com/oauth/authorize";
    constexpr const char *TOKEN_URL = "https://www.pathofexile.com/oauth/token";
    constexpr const char *CLIENT_ID = "acquisition";
    constexpr const std::array SCOPE = {"account:leagues", "account:stashes", "account:characters"};

    // Acquisition as currently approved uses a plain HTTP callback.
    // This should be changed to HTTPS or a private URI scheme at some point.
    constexpr const char *CALLBACK_HOST = "127.0.0.1";
    constexpr const char *CALLBACK_PATH = "/auth/path-of-exile";

}; // namespace

OAuthManager::OAuthManager(NetworkManager &network_manager, QObject *parent)
    : QObject(nullptr)
{
    initOAuth();
    initHandler();

    // Set the network access manager.
    m_oauth.setNetworkAccessManager(&network_manager);

    // Setup a connection to grab the details of a token before granted() has been received.
    connect(&m_handler,
            &QAbstractOAuthReplyHandler::tokensReceived,
            this,
            &OAuthManager::receiveToken);

    // Connect the oauth code flow.
    connect(&m_oauth, &QAbstractOAuth::authorizeWithBrowser, this, &QDesktopServices::openUrl);
    connect(&m_oauth, &QAbstractOAuth::granted, this, &OAuthManager::receiveGrant);
    connect(&m_oauth,
            &QAbstractOAuth::requestFailed,
            this,
            [this](const QAbstractOAuth::Error error) {
                spdlog::error("OAuth request failed: error {}", static_cast<int>(error));
            });

    // Enable automatic token refresh.
    m_oauth.setAutoRefresh(true);
}

void OAuthManager::initOAuth()
{
    // Setup the scope tokens.
    const QSet<QByteArray> scope_tokens{SCOPE.begin(), SCOPE.end()};

    // Setup OAuth for the Path of Exile API.
    m_oauth.setAuthorizationUrl(QUrl{AUTHORIZATION_URL});
    m_oauth.setTokenUrl(QUrl{TOKEN_URL});
    m_oauth.setClientIdentifier(CLIENT_ID);
    m_oauth.setPkceMethod(QOAuth2AuthorizationCodeFlow::PkceMethod::S256);
    m_oauth.setRequestedScopeTokens(scope_tokens);
    m_oauth.setReplyHandler(&m_handler);

    // Hack to make token refresh work correctly.
    m_oauth.setModifyParametersFunction(
        [](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            // During token refresh, if the client_secret parameter is present but
            // empty, then we have to remove it to avoid a server error.
            if (stage == QAbstractOAuth::Stage::RefreshingAccessToken) {
                parameters->remove("client_secret");
            }
        });
}

void OAuthManager::initHandler()
{
    // Setup the callback handler.
    m_handler.setCallbackHost(CALLBACK_HOST);
    m_handler.setCallbackPath(CALLBACK_PATH);
    m_handler.setCallbackText("Acquisition has been authorized.");
}

void OAuthManager::receiveToken(const QVariantMap &tokens)
{
    m_token = OAuthToken();
    m_token.access_token = tokens["access_token"].toString();
    m_token.expires_in = tokens["expires_in"].toLongLong();
    m_token.refresh_token = tokens["refresh_token"].toString();
    m_token.scope = tokens["scope"].toString();
    m_token.sub = tokens["sub"].toString();
    m_token.token_type = tokens["token_type"].toString();
    m_token.username = tokens["username"].toString();
    spdlog::info("OAuth: tokens recieved for {}", m_token.username);
}

void OAuthManager::receiveGrant()
{
    m_handler.close();
    auto requested = m_oauth.requestedScopeTokens();
    const auto &granted = m_oauth.grantedScopeTokens();
    const auto &diff = requested.subtract(granted);
    if (!diff.empty()) {
        spdlog::error("OAuth: was not granted these requested scopes: {}",
                      QStringList(diff.begin(), diff.end()).join(", "));
    }
    spdlog::info("OAuth access was granted.");
    emit grantAccess(m_token);
    emit isAuthenticatedChanged();
}

void OAuthManager::setToken(const OAuthToken &token)
{
    if (token.refresh_token.isEmpty()) {
        spdlog::error("OAuth: trying to refresh with an empty refresh token");
    } else {
        m_oauth.setRefreshToken(token.refresh_token);
        m_oauth.refreshTokens();
    }
}

void OAuthManager::initLogin()
{
    spdlog::info("OAuth access requested.");
    m_oauth.grant();
}
