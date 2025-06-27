// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QMetaEnum>
#include <QMetaObject>
#include <QString>
#include <QStringList>

namespace util {

    template<typename EnumType>
    QString toString(EnumType value)
    {
        const QMetaEnum &metaEnum = QMetaEnum::fromType<EnumType>();
        const char *key = metaEnum.valueToKey(static_cast<int>(value));
        return QString::fromUtf8(key);
    }

    template<typename EnumType>
    QStringList toStringList()
    {
        const QMetaEnum &metaEnum = QMetaEnum::fromType<EnumType>();
        QStringList keys;
        keys.reserve(metaEnum.keyCount());
        for (auto i = 0; i < metaEnum.keyCount(); ++i) {
            auto key = metaEnum.key(i);
            keys.append(QString::fromUtf8(key));
        }
        return keys;
    }

} // namespace util
