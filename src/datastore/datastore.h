// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QMutex>
#include <QObject>
#include <QSet>
#include <QSqlDatabase>
#include <QString>

class DataStore : public QObject
{
    Q_OBJECT

public:
    DataStore(const QString &filename, QObject *parent = nullptr);
    ~DataStore();

protected:
    QString getThreadLocalConnectionName() const;
    QSqlDatabase getThreadLocalDatabase();

    void createTable(const QString &table, const QStringList &columns);
    void createIndexes(const QString &table, const QStringList &columns);

    const QString m_filename;

private:
    mutable QMutex m_mutex;
    mutable QSet<QString> m_connections;
};
