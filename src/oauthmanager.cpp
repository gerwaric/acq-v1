// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "oauthmanager.h"

#include "networkmanager.h"
#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

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

    // This list is complete as of Qt 6.9.1:
    // See https://doc.qt.io/qt-6/qabstractoauth.html#Error-enum
    // clang-format off
    constexpr std::array<std::pair<QAbstractOAuth::Error, const char *>, 8> KNOWN_OAUTH_ERRORS{{
        {QAbstractOAuth::Error::NoError, "NoError"}, //	0	No error has ocurred.
        {QAbstractOAuth::Error::NetworkError, "NetworkError"}, //	1	Failed to connect to the server.
        {QAbstractOAuth::Error::ServerError, "ServerError"}, //	2	The server answered the request with an error, or its response was not successfully received (for example, due to a state mismatch).
        {QAbstractOAuth::Error::OAuthTokenNotFoundError, "OAuthTokenNotFoundError"}, //	3	The server's response to a token request provided no token identifier.
        {QAbstractOAuth::Error::OAuthTokenSecretNotFoundError, "OAuthTokenSecretNotFoundError"}, //	4	The server's response to a token request provided no token secret.
        {QAbstractOAuth::Error::OAuthCallbackNotVerified, "OAuthCallbackNotVerified"}, //	5	The authorization server has not verified the supplied callback URI in the request. This usually happens when the provided callback does not match with the callback supplied during client registration.
        {QAbstractOAuth::Error::ClientError, "ClientError"}, // (since Qt 6.9)	6	An error that is attributable to the client application (e.g. missing configuration or attempting a request in a state where it's not allowed). Currently used by QOAuth2DeviceAuthorizationFlow.
        {QAbstractOAuth::Error::ExpiredError, "ExpiredError"}} // (since Qt 6.9)	7	A token has expired. Currently used by QOAuth2DeviceAuthorizationFlow.
    };
    // clang-format on

}; // namespace

OAuthManager::OAuthManager(NetworkManager &network_manager, QObject *parent)
    : QObject(nullptr)
{
    // Set the network access manager.
    m_oauth.setNetworkAccessManager(&network_manager);

    // Create the scope tokens.
    const QSet<QByteArray> scope_tokens{SCOPE.begin(), SCOPE.end()};

    // Setup OAuth for the Path of Exile API.
    m_oauth.setAuthorizationUrl(QUrl{AUTHORIZATION_URL});
    m_oauth.setTokenUrl(QUrl{TOKEN_URL});
    m_oauth.setClientIdentifier(CLIENT_ID);
    m_oauth.setPkceMethod(QOAuth2AuthorizationCodeFlow::PkceMethod::S256);
    m_oauth.setRequestedScopeTokens(scope_tokens);
    m_oauth.setReplyHandler(&m_handler);
    m_oauth.setAutoRefresh(true);

    // Setup a hack to make token refresh work correctly.
    m_oauth.setModifyParametersFunction(
        [](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
            // During token refresh, if the client_secret parameter is present but
            // empty, then the POE server will give us an error (as of 3.26).
            if (stage == QAbstractOAuth::Stage::RefreshingAccessToken) {
                parameters->remove("client_secret");
            }
        });

    // Setup the handler.
    m_handler.setCallbackHost(CALLBACK_HOST);
    m_handler.setCallbackPath(CALLBACK_PATH);
    m_handler.setCallbackText("Acquisition has been authorized.");

    // Setup a connection to grab the details of a token before granted() has been received.
    connect(&m_handler,
            &QAbstractOAuthReplyHandler::tokensReceived,
            this,
            &OAuthManager::receiveToken);

    // Connect the oauth code flow.
    connect(&m_oauth, &QAbstractOAuth::authorizeWithBrowser, this, &QDesktopServices::openUrl);
    connect(&m_oauth, &QAbstractOAuth::granted, this, &OAuthManager::receiveGrant);

    // Add err logging.
    connect(&m_oauth, &QAbstractOAuth::requestFailed, this, [](const QAbstractOAuth::Error error) {
        for (const auto &[known_error, name] : KNOWN_OAUTH_ERRORS) {
            if (error == known_error) {
                spdlog::error("OAuth: request failed: error {} ({})", static_cast<int>(error), name);
                return;
            }
        }
        spdlog::error("OAuth: request failed: error {} (unknown error)", static_cast<int>(error));
    });
    connect(&m_oauth,
            &QAbstractOAuth2::serverReportedErrorOccurred,
            this,
            [](const QString &error, const QString &errorDescription, const QUrl &uri) {
                spdlog::error("Oauth: server reported error: '{}' ({}): {}",
                              error,
                              errorDescription,
                              uri.toDisplayString());
            });
}

void OAuthManager::receiveToken(const QVariantMap &tokens)
{
    m_token = OAuthToken();
    // clang-format off
    m_token.access_token  = tokens["access_token"].toString();
    m_token.expires_in    = tokens["expires_in"].toLongLong();
    m_token.refresh_token = tokens["refresh_token"].toString();
    m_token.scope         = tokens["scope"].toString();
    m_token.sub           = tokens["sub"].toString();
    m_token.token_type    = tokens["token_type"].toString();
    m_token.username      = tokens["username"].toString();
    // clang-format on
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
