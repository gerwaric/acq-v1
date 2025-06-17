// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "networkmanager.h"

#include "app_version.h"

#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager(parent) {};

void NetworkManager::setBearerToken(const QString &token)
{
    m_bearer_token = token.isEmpty() ? "" : ("Bearer " + token.toUtf8());
}

QNetworkReply *NetworkManager::createRequest(QNetworkAccessManager::Operation op,
                                             const QNetworkRequest &originalRequest,
                                             QIODevice *outgoingData)
{
    // Always set the user agent.
    QNetworkRequest request(originalRequest);
    request.setRawHeader("User-Agent", APP_USER_AGENT);

    // Conditionally add a bearer token (for API calls).
    if (request.url().host() == "api.pathofexile.com") {
        if (m_bearer_token.isEmpty()) {
            spdlog::error("API calls may fail because the bearer token is empty.");
        };
        request.setRawHeader("Authorization", m_bearer_token);
    };

    // Let the base class handle the rest.
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}
