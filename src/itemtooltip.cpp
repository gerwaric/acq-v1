// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "itemtooltip.h"

ItemTooltip::ItemTooltip(TreeModel &model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{}

void ItemTooltip::onItemChanged(const QModelIndex &current, const QModelIndex &previous)
{
    const auto *node = m_model.getNode(current);

    m_itemNameFirstLine = "Ues";
}
