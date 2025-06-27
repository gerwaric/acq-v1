// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "networkmanager.h"
#include "oauthtoken.h"

#include <QJSEngine>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>

#include <QtQmlIntegration/qqmlintegration.h>

class OAuthManager : public QObject
{
    Q_OBJECT

public:
    explicit OAuthManager(NetworkManager &manager, QObject *parent = nullptr);

    void setToken(const OAuthToken &token);

    void initLogin();
    QString username() const { return m_token.username; };

private slots:
    void receiveToken(const QVariantMap &tokens);
    void receiveGrant();

signals:
    void grantAccess(const OAuthToken &token);
    void isAuthenticatedChanged();

private:
    QOAuth2AuthorizationCodeFlow m_oauth;
    QOAuthHttpServerReplyHandler m_handler;

    void initOAuth();

    bool m_authenticated{false};
    OAuthToken m_token;
};
