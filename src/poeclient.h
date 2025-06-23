// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poe.h"
#include "util/json.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QtQmlIntegration/qqmlintegration.h>

class PoeClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum class Realm { PC, XBOX, SONY, POE2 };
    Q_ENUM(Realm);

    explicit PoeClient(QObject *parent = nullptr);

    // https://www.pathofexile.com/developer/docs/reference#leagues-list
    Q_INVOKABLE void listLeagues(const QString &realm);

    // https://www.pathofexile.com/developer/docs/reference#characters-list
    Q_INVOKABLE void listCharacters(const QString &realm);

    // https://www.pathofexile.com/developer/docs/reference#stashes-list
    Q_INVOKABLE void listStashes(const QString &realm, const QString &league);

    // https://www.pathofexile.com/developer/docs/reference#characters-get
    Q_INVOKABLE void getCharacter(const QString &realm, const QString &name);

    // https://www.pathofexile.com/developer/docs/reference#stashes-get
    Q_INVOKABLE void getStash(const QString &realm,
                              const QString &league,
                              const QString &stash_id,
                              const QString &substash_id);

signals:

    // Emitted when we want to send an API request.
    void requestReady(QString endpoint,
                      QNetworkRequest request,
                      std::function<void(QNetworkReply *)>);

    // Emitted when a call to list leagues has finished.
    void leagueListReceived(QString realm, std::vector<poe::League> leagues, QByteArray data);

    // Emitted when a call to list characters has finished.
    void characterListReceived(QString realm,
                               std::vector<poe::Character> characters,
                               QByteArray data);

    // Emitted when a call to list stashes has finished.
    void stashListReceived(QString realm,
                           QString league,
                           std::vector<poe::StashTab> stashes,
                           QByteArray data);

    // Emitted when a character has been fetched.
    void characterReceived(QString realm,
                           QString name,
                           std::optional<poe::Character> character,
                           QByteArray data);

    // Emitted when a stash has been fetched.
    void stashReceived(QString realm,
                       QString league,
                       QString stash_id,
                       QString substash_id,
                       std::optional<poe::StashTab> stash,
                       QByteArray data);

private:
    // These wrapper structs are only user internally to unpack api responses.
    // There's no need for the caller to know about them.

    struct LeagueListWrapper
    {
        std::vector<poe::League> leagues;
    };

    struct CharacterListWrapper
    {
        std::vector<poe::Character> characters;
    };

    struct StashListWrapper
    {
        std::vector<poe::StashTab> stashes;
    };

    struct CharacterWrapper
    {
        std::optional<poe::Character> character;
    };

    struct StashTabWrapper
    {
        std::optional<poe::StashTab> stash;
    };

    unsigned m_pendingCharacterRequests{0};
    unsigned m_pendingStashRequests{0};

    // Convenience function to add realm to request urls.
    [[nodiscard]] static bool checkRealm(const QString &realm, const char *endpoint);

    // Convenience function to get json data from a network reply.
    template<typename WrapperType>
    [[nodiscard]] static std::pair<WrapperType, QByteArray> unpack(QNetworkReply *reply)
    {
        const QByteArray data = reply ? reply->readAll() : "";
        const auto [parsed, ok] = json::parse_permissive<WrapperType>(data);
        if (!ok) {
            spdlog::error("PoE: error unapacking {}", typeid(WrapperType).name());
        }
        reply->deleteLater();
        return {parsed, data};
    }
};
