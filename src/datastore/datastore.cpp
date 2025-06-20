// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "datastore.h"

#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG // prevents an unused header warning in Qt Creator

#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>

DataStore::DataStore(const QString &filename, QObject *parent)
    : QObject(parent)
    , m_filename(filename)
{}

DataStore::~DataStore()
{
    QMutexLocker locker(&m_mutex);
    const auto &connections = m_connections;
    for (const QString &connection : connections) {
        if (QSqlDatabase::contains(connection)) {
            spdlog::info("DataStore: removing {}", connection);
            QSqlDatabase::database(connection, false).close();
            QSqlDatabase::removeDatabase(connection);
        }
    }
    m_connections.clear();
}

QString DataStore::getThreadLocalConnectionName() const
{
    const QString file_name = QFileInfo(m_filename).fileName();
    const quintptr thread_id = reinterpret_cast<quintptr>(QThread::currentThread());
    return QString("sqlite-%1-%2").arg(file_name).arg(thread_id);
}

QSqlDatabase DataStore::getThreadLocalDatabase()
{
    QString connection = getThreadLocalConnectionName();
    if (!QSqlDatabase::contains(connection)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection);
        db.setDatabaseName(m_filename);
        if (!db.open()) {
            const QString message = db.lastError().text();
            spdlog::error("Failed to open database {}: {}", m_filename, message);
        } else {
            // Add the connection to the list so we can close it later.
            QMutexLocker locker(&m_mutex);
            m_connections.insert(connection);
        }
    }
    return QSqlDatabase::database(connection);
}

void DataStore::createTable(const QString &table, const QStringList &columns)
{
    const QString cols = columns.join(", ");
    const QString stmt = QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(table, cols);

    spdlog::trace("DataStore: creating {} with ({})", table, cols);

    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);
    query.prepare(stmt);

    if (!query.exec()) {
        const QString message = query.lastError().text();
        spdlog::error("DataStore: failed to create {}: {}", table, message);
    }
}

void DataStore::createIndexes(const QString &table, const QStringList &columns)
{
    auto db = getThreadLocalDatabase();
    auto query = QSqlQuery(db);

    for (const auto &column : columns) {
        const QString idx = QString("idx_%1_%2").arg(table, column);

        query.prepare("SELECT name FROM sqlite_master WHERE type='index' AND name = ?");
        query.bindValue(0, idx);
        if (!query.exec()) {
            const QString message = query.lastError().text();
            spdlog::error("DataStore: error checking index '{}': {}", idx, message);
            continue;
        }

        if (query.next()) {
            spdlog::debug("DataStore: index already exists: {}", idx);
            continue;
        }

        query.prepare(QString("CREATE INDEX %1 ON %2(%3)").arg(idx, table, column));
        if (!query.exec()) {
            const QString message = query.lastError().text();
            spdlog::error("DataStore: failed to create '{}': {}", idx, message);
            continue;
        }
    }
}
