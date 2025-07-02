// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>

#include <QtQmlIntegration/qqmlintegration.h>

class ItemTooltip : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString itemNameFirstLine READ getItemNameFirstLine NOTIFY tooltipChanged)
    Q_PROPERTY(QString itemNameSecondLine READ getItemNameSecondLine NOTIFY tooltipChanged)

public:
    explicit ItemTooltip(QObject *parent = nullptr);

    QString getItemNameFirstLine() const { return m_itemNameFirstLine; }
    QString getItemNameSecondLine() const { return m_itemNameSecondLine; }

signals:
    void tooltipChanged();

public slots:
    void onItemChanged();

private:
    QString m_itemNameFirstLine{"First"};
    QString m_itemNameSecondLine{"Second"};
};
