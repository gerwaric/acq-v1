// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QHttpHeaders>
#include <QIODevice>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QStringList>

class NetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    void setBearerToken(const QString &token);

    static void logRequest(const QNetworkRequest &request);
    static void logReply(const QNetworkReply *reply);

protected:
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op,
                                 const QNetworkRequest &originalRequest,
                                 QIODevice *outgoingData = nullptr) override;

private:
    QNetworkDiskCache m_diskCache;
    QByteArray m_bearerToken;

    using AttributeGetter = std::function<QVariant(QNetworkRequest::Attribute)>;

    static void logAttributes(const QString &name, AttributeGetter attrs);
    static void logHeaders(const QString &name, const QHttpHeaders &headers);
};
