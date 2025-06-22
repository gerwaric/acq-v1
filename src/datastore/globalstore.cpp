// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "globalstore.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG); // Prevents an unused header warning in Qt Creator.

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

GlobalStore::GlobalStore(QObject *parent)
    : DataStore(getPath("global.db"), parent)
{
    createTable("data", {"key TEXT PRIMARY KEY", "value BLOB"});
}

void GlobalStore::set(const QString &key, const QVariant &value)
{
    QSqlDatabase db = getThreadLocalDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO data (key, value) VALUES (?, ?)");
    query.bindValue(0, key);
    query.bindValue(1, value);
    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("GlobalStore: error writing '{}': {}", key, message);
    }
}

QVariant GlobalStore::get(const QString &key)
{
    QSqlDatabase db = getThreadLocalDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT value FROM data WHERE key = ?");
    query.bindValue(0, key);
    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("GlobalStore: error reading '{}': {}", key, message);
        return QVariant();
    }
    if (!query.next()) {
        if (!query.isActive()) {
            const QString message = query.lastError().text();
            spdlog::error("GlobalStore: error getting '{}': {}", key, message);
        }
        return QVariant();
    }
    return query.value(0);
}

QString GlobalStore::getPath(const QString &filename)
{
    const QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    return dir.absoluteFilePath(filename);
}
