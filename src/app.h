// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "app_version.h"
#include "datastore/globaldatastore.h"
#include "networkmanager.h"
#include "oauthmanager.h"
#include "ratelimit/ratelimiter.h"

#include <QObject>
#include <QString>

#include <QtQmlIntegration/qqmlintegration.h>

class App : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticationStateChanged)
    Q_PROPERTY(QString username READ username NOTIFY authenticationStateChanged)
    Q_PROPERTY(QString version READ version CONSTANT);

public:
    App(QObject *parent = nullptr);

    Q_INVOKABLE void authenticate();

    bool isAuthenticated() const { return m_authenticated; };
    QString username() const { return m_username; };
    QString version() const { return APP_VERSION_STRING; };

signals:
    void authenticationStateChanged();

private slots:
    void authenticated(const OAuthToken &token);

private:
    NetworkManager m_networkManager;
    GlobalDataStore m_globalData;
    OAuthManager m_oauthManager;
    RateLimiter m_rateLimiter;

    bool m_authenticated;
    QString m_username;
};
