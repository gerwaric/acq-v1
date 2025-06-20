// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>

#include <QNetworkReply>
#include <QNetworkRequest>

class PoeClient : public QObject
{
    Q_OBJECT

public:
    enum class Realm { PC, XBOX, SONY, POE2 };
    Q_ENUM(Realm);

    explicit PoeClient(QObject *parent = nullptr);

    // https://www.pathofexile.com/developer/docs/reference#leagues-list
    void listLeagues(const QString &realm);

    // https://www.pathofexile.com/developer/docs/reference#characters-list
    void listCharacters(const QString &realm);

    // https://www.pathofexile.com/developer/docs/reference#stashes-list
    void listStashes(const QString &realm, const QString &league);

    // https://www.pathofexile.com/developer/docs/reference#characters-get
    void getCharacter(const QString &realm, const QString &name);

    // https://www.pathofexile.com/developer/docs/reference#stashes-get
    void getStash(const QString &realm,
                  const QString &league,
                  const QString &stash_id,
                  const QString &substash_id);

signals:

    // Emitted when a request has been made
    void sendRequest(QString endpoint,
                     QNetworkRequest request,
                     std::function<void(QNetworkReply *)>);

    // Emitted when a reply has been received.
    void leagueListReceived(QString realm, std::shared_ptr<QByteArray> data);
    void characterListReceived(QString realm, std::shared_ptr<QByteArray> data);
    void stashListReceived(QString realm, QString league, std::shared_ptr<QByteArray> data);
    void characterReceived(QString realm, QString name, std::shared_ptr<QByteArray> data);
    void stashReceived(QString realm,
                       QString league,
                       QString stash_id,
                       QString substash_id,
                       std::shared_ptr<QByteArray> data);

private:
    // Convenience function to add realm to request urls.
    [[nodiscard]] static bool checkRealm(const QString &realm, const char *endpoint);

    // Convenience function to get json data from a network reply.
    [[nodiscard]] static std::shared_ptr<QByteArray> getData(QNetworkReply *reply);
};
