// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <poe/types/character.h>
#include <poe/types/league.h>
#include <poe/types/stashtab.h>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

class PoeClient : public QObject
{
    Q_OBJECT

public:
    enum class Realm { pc, xbox, sony, poe2 };
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

    static std::vector<poe::League> unwrapLeageList(const QByteArray &data);
    static std::vector<poe::Character> unwrapCharacterList(const QByteArray &data);
    static std::vector<poe::StashTab> unwrapStashList(const QByteArray &data);

    static std::optional<poe::Character> unwrapCharacter(const QByteArray &data);
    static std::optional<poe::StashTab> unwrapStash(const QByteArray &data);

signals:

    // Emitted when we want to send an API request.
    void requestReady(QString endpoint,
                      QNetworkRequest request,
                      std::function<void(QNetworkReply *)>);

    // Emitted when a call to list leagues has finished.
    void leagueListDataReceived(QString realm, QByteArray data);

    // Emitted when a call to list characters has finished.
    void characterListDataReceived(QString realm, QByteArray data);

    // Emitted when a call to list stashes has finished.
    void stashListDataReceived(QString realm, QString league, QByteArray data);

    // Emitted when a character has been fetched.
    void characterDataReceived(QString realm, QString name, QByteArray data);

    // Emitted when a stash has been fetched.
    void stashDataReceived(
        QString realm, QString league, QString stash_id, QString substash_id, QByteArray data);

private:
    // These wrapper structs are only user internally to unpack api responses.
    // There's no need for the caller to know about them.

    /*
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
    */

    unsigned m_pendingCharacterRequests{0};
    unsigned m_pendingStashRequests{0};

    // Convenience function to add realm to request urls.
    [[nodiscard]] static bool checkRealm(const QString &realm, const char *endpoint);
};
