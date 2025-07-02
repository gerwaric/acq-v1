// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "app_version.h"
#include "itemtooltip.h"
#include "networkmanager.h"
#include "oauthmanager.h"
#include "poeclient.h"

#include "datastore/globalstore.h"
#include "datastore/userstore.h"
#include "model/treemodel.h"
#include "ratelimit/ratelimiter.h"

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QObject>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QString>
#include <QStringList>

#include <QtQmlIntegration/qqmlintegration.h>

#include <vector>

class App : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString logLevel READ getLogLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString rateLimitStatus MEMBER m_rateLimitStatus NOTIFY rateLimitStatusChanged)

    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticationStateChanged)
    Q_PROPERTY(QString username READ getUsername NOTIFY authenticationStateChanged)

    Q_PROPERTY(QItemSelectionModel *itemSelectionModel READ getItemSelectionModel CONSTANT)
    Q_PROPERTY(ItemTooltip *itemTooltip READ getItemTooltip CONSTANT)
    Q_PROPERTY(QSqlQueryModel *characterModel READ getCharacterModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *stashModel READ getStashModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *itemsModel READ getItemModel CONSTANT)

public:
    App(QObject *parent = nullptr);

    Q_INVOKABLE void authenticate();
    Q_INVOKABLE void getCharacter();
    Q_INVOKABLE void getAllCharacters();
    Q_INVOKABLE void getAllStashes();
    Q_INVOKABLE void loadSelectedCharacters();
    Q_INVOKABLE void loadSelectedStashes();

    QString getLogLevel() const;
    Q_INVOKABLE void setLogLevel(const QString &level);

    QString version() const { return APP_VERSION_STRING; };
    bool isAuthenticated() const { return m_authenticated; };
    QString getUsername() const { return m_username; };

    Q_INVOKABLE QStringList getRealmNames() const;
    Q_INVOKABLE QStringList getLeagueNames(const QString &realm) const;
    Q_INVOKABLE void loadItems(const QString &realm, const QString &league);

    QStringList getCharacterNames() const;
    QStringList getStashNames() const;
    QString getSelectedItemIconUrl() const { return m_selectedItemImageUrl; }

    // property getters
    QItemSelectionModel *getItemSelectionModel() { return &m_itemSelectionModel; }
    ItemTooltip *getItemTooltip() { return &m_tooltip; }
    QSqlQueryModel *getCharacterModel() { return &m_characterTableModel; }
    QSqlQueryModel *getStashModel() { return &m_stashTableModel; }
    QAbstractItemModel *getItemModel() { return &m_itemModel; }

signals:
    void logLevelChanged();
    void authenticationStateChanged();

    void leaguesUpdated();
    void charactersUpdated();
    void stashesUpdated();
    void rateLimitStatusChanged();

private slots:
    void accessGranted(const OAuthToken &token);

    void handleLeagueList(const QString &realm,
                          const std::vector<poe::League> &leagues,
                          const QByteArray &data);

    void handleCharacterList(const QString &realm,
                             const std::vector<poe::Character> &characters,
                             const QByteArray &data);

    //void handleStashList(const QString &realm, const QString &league, const poe::StashTab &stashes);

    void rateLimited(int seconds, const QString &policy_name);

private:
    NetworkManager m_networkManager;
    GlobalStore m_globalStore;
    OAuthManager m_oauthManager;
    RateLimiter m_rateLimiter;
    PoeClient m_client;

    ItemTooltip m_tooltip;

    std::unique_ptr<UserStore> m_clientStore;

    TreeModel m_itemModel;
    QItemSelectionModel m_itemSelectionModel;
    QSqlQueryModel m_characterTableModel;
    QSqlQueryModel m_stashTableModel;

    bool m_authenticated{false};
    QString m_debugLevel;
    QString m_realm;
    QString m_username;
    QString m_league;
    QString m_character;
    QString m_rateLimitStatus;

    QString m_selectedItemImageUrl;

    std::vector<poe::League> m_leagueList;
    std::vector<poe::Character> m_characterList;
    std::vector<poe::StashTab> m_stashList;

};
