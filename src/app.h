// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "app_version.h"
#include "networkmanager.h"
#include "oauthmanager.h"
#include "poe.h"
#include "poeclient.h"

#include "datastore/globalstore.h"
#include "datastore/userstore.h"
#include "ratelimit/ratelimiter.h"

#include <QObject>
#include <QString>

#include <QtQmlIntegration/qqmlintegration.h>

#include <QSqlTableModel>

class App : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString debugLevel READ getDebugLevel WRITE setDebugLevel NOTIFY debugLevelChanged)
    Q_PROPERTY(QString realm MEMBER m_realm)
    Q_PROPERTY(QString league MEMBER m_league)
    Q_PROPERTY(QString character MEMBER m_character)
    Q_PROPERTY(QString rateLimitStatus MEMBER m_rateLimitStatus NOTIFY rateLimitStatusChanged)

    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticationStateChanged)
    Q_PROPERTY(QString username READ getUsername NOTIFY authenticationStateChanged)

    Q_PROPERTY(QStringList leagues READ getLeagueNames NOTIFY leaguesUpdated)
    Q_PROPERTY(QStringList characters READ getCharacterNames NOTIFY charactersUpdated)
    Q_PROPERTY(QStringList stashes READ getStashNames NOTIFY stashesUpdated)

    Q_PROPERTY(QSqlTableModel *itemsModel READ getItemsModel CONSTANT);

public:
    App(QObject *parent = nullptr);

    Q_INVOKABLE void authenticate();
    Q_INVOKABLE void listLeagues();
    Q_INVOKABLE void listCharacters();
    Q_INVOKABLE void listStashes();
    Q_INVOKABLE void getCharacter();
    Q_INVOKABLE void getAllCharacters();
    Q_INVOKABLE void getAllStashes();

    Q_INVOKABLE void loadCurrentCharacter();

    QString getDebugLevel() const { return m_debugLevel; };
    void setDebugLevel(const QString &level) {};

    QString version() const { return APP_VERSION_STRING; };
    bool isAuthenticated() const { return m_authenticated; };
    QString getUsername() const { return m_username; };

    QStringList getLeagueNames() const;
    QStringList getCharacterNames() const;
    QStringList getStashNames() const;

    QSqlTableModel *getItemsModel() { return m_itemsModel; };

signals:
    void debugLevelChanged();
    void authenticationStateChanged();
    void leaguesUpdated();
    void charactersUpdated();
    void stashesUpdated();
    void rateLimitStatusChanged();

private slots:
    void accessGranted(const OAuthToken &token);

    void handleLeagueList(poe::LeagueListPtr leagues);
    void handleCharacterList(poe::CharacterListPtr characters);
    void handleCharacter(poe::CharacterPtr character);
    void handleStashList(poe::StashListPtr stashes);

    void rateLimited(int seconds, const QString &policy_name);

private:
    NetworkManager m_networkManager;
    GlobalStore m_globalStore;
    OAuthManager m_oauthManager;
    RateLimiter m_rateLimiter;
    PoeClient m_poeClient;

    UserStore *m_userStore{nullptr};

    bool m_authenticated{false};
    QString m_debugLevel;
    QString m_realm;
    QString m_username;
    QString m_league;
    QString m_character;
    QString m_rateLimitStatus;

    std::vector<poe::League> m_leagueList;
    std::vector<poe::Character> m_characterList;
    std::vector<poe::StashTab> m_stashList;

    void createItemsStore();
    QSqlTableModel *m_itemsModel;
};
