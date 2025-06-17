// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "globaldatastore.h"

#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

GlobalDataStore::GlobalDataStore(QObject *parent)
    : DataStore(getPath("global.db"), parent)
{
    QSqlDatabase db = getThreadLocalDatabase();
    QSqlQuery query(db);
    query.prepare("CREATE TABLE IF NOT EXISTS data (key TEXT PRIMARY KEY, value BLOB)");
    if (!query.exec()) {
        spdlog::error("Failed to create global datastore: {}", query.lastError().text());
    };
}

void GlobalDataStore::set(const QString &key, const QVariant &value)
{
    QSqlDatabase db = getThreadLocalDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO data (key, value) VALUES (?, ?)");
    query.bindValue(0, key);
    query.bindValue(1, value);
    if (!query.exec()) {
        spdlog::error("Error writing '{}' to global datastore: {}", key, query.lastError().text());
    };
}

QVariant GlobalDataStore::get(const QString &key)
{
    QSqlDatabase db = getThreadLocalDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT value FROM data WHERE key = ?");
    query.bindValue(0, key);
    if (!query.exec()) {
        spdlog::error("Error reading '{}' from global datastore: {}", key, query.lastError().text());
        return QVariant();
    };
    if (!query.next()) {
        if (!query.isActive()) {
            spdlog::error("Error getting '{}' from global datastore: {}",
                          key,
                          query.lastError().text());
        };
        return QVariant();
    };
    return query.value(0);
}

QString GlobalDataStore::getPath(const QString &filename)
{
    const QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    return dir.absoluteFilePath(filename);
}
