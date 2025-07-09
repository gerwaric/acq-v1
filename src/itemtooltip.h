// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "model/itemdata.h"

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
    Q_PROPERTY(QString headerMiddle MEMBER m_headerMiddle CONSTANT)
    Q_PROPERTY(QString headerRight MEMBER m_headerRight CONSTANT)

    Q_PROPERTY(QString headerOverlayLeft MEMBER m_headerLeftOverlay CONSTANT)
    Q_PROPERTY(QString headerOverlayRight MEMBER m_headerRightOverlay CONSTANT)

    Q_PROPERTY(QSize headerSize MEMBER m_headerSize CONSTANT)
    Q_PROPERTY(QSize headerOverlaySize MEMBER m_headerOverlaySize CONSTANT)

    Q_PROPERTY(QStringList tooltipSections MEMBER m_tooltipSections CONSTANT)

    Q_PROPERTY(QString separatorUrl MEMBER m_separatorUrl CONSTANT)

public:
    explicit ItemTooltip(const ItemData &item, QObject *parent = nullptr);

private:
    QString renderDisplayText(const ItemData &item);
    QString renderModList(const QStringList &mods, const char *color);

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

    QStringList m_tooltipSections;

    QString m_separatorUrl;
    QString m_propertiesText1;
    QString m_propertiesText2;
    QString m_requirementsTest;
    QString m_implicitsText;
    QString m_enchantmentsText;
    QString m_modifiersText;
};
