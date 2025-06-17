// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "datastore.h"

#include "util/spdlog_qt.h"

ACQUISITION_USE_SPDLOG

#include <QFileInfo>
#include <QSqlError>
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
        };
    };
    m_connections.clear();
}

QString DataStore::getThreadLocalConnectionName() const
{
    // Create a database connection name for this thread.
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
            spdlog::error("Failed to open database {}: {}", m_filename, db.lastError().text());
        } else {
            // Add the connection to the list so we can close it later.
            QMutexLocker locker(&m_mutex);
            m_connections.insert(connection);
        }
    };
    return QSqlDatabase::database(connection);
}
