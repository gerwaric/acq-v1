// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "model/iteminfo.h"

#include <QObject>
#include <QSize>
#include <QString>

#include <QtQmlIntegration/qqmlintegration.h>

class ItemTooltip : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString icon MEMBER m_icon CONSTANT)
    Q_PROPERTY(QString background MEMBER m_background CONSTANT)
    Q_PROPERTY(QString frameColor MEMBER m_frameColor CONSTANT)

    Q_PROPERTY(QString headerLeft MEMBER m_headerLeft CONSTANT)
    Q_PROPERTY(QString headerLeftOverlay MEMBER m_headerLeftOverlay CONSTANT)
    Q_PROPERTY(QString headerMiddle MEMBER m_headerMiddle CONSTANT)
    Q_PROPERTY(QString headerRight MEMBER m_headerRight CONSTANT)
    Q_PROPERTY(QString headerRightOverlay MEMBER m_headerRightOverlay CONSTANT)

    Q_PROPERTY(QSize headerSize MEMBER m_headerSize CONSTANT)
    Q_PROPERTY(QSize headerOverlaySize MEMBER m_headerOverlaySize CONSTANT)

public:
    explicit ItemTooltip(const ItemInfo &item_info, QObject *parent = nullptr);

private:
    QString m_name;
    QString m_icon;
    QString m_background;
    QString m_frameColor;

    QString m_headerLeft;
    QString m_headerLeftOverlay;
    QString m_headerMiddle;
    QString m_headerRight;
    QString m_headerRightOverlay;

    QSize m_headerSize;
    QSize m_headerOverlaySize;
};
