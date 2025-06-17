// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "datastore.h"

#include "util/json.h"

class GlobalDataStore : public DataStore
{
    Q_OBJECT

public:
    GlobalDataStore(QObject *parent = nullptr);

    void set(const QString &key, const QVariant &value);
    QVariant get(const QString &key);

    template<typename T>
    void store(const QString &key, const T &value)
    {
        const QString strval = json::toString(value);
        set(key, strval);
    };

    template<typename T>
    std::pair<T, bool> retrieve(const QString &key)
    {
        const QString strval = get(key).toString();
        return json::from_json<T>(strval, json::STRICT);
    };

private:
    static QString getPath(const QString &filename);
};
