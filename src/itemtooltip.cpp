// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "itemtooltip.h"

#include "poe/types/frametype.h"

#include <QDir>
#include <QImage>
#include <QResource>

namespace {

    constexpr int HEADER_SINGLELINE_WIDTH = 29;
    constexpr int HEADER_SINGLELINE_HEIGHT = 34;
    constexpr QSize HEADER_SINGLELINE_SIZE(HEADER_SINGLELINE_WIDTH, HEADER_SINGLELINE_HEIGHT);

    constexpr int HEADER_DOUBLELINE_WIDTH = 44;
    constexpr int HEADER_DOUBLELINE_HEIGHT = 54;
    constexpr QSize HEADER_DOUBLELINE_SIZE(HEADER_DOUBLELINE_WIDTH, HEADER_DOUBLELINE_HEIGHT);

    constexpr int HEADER_OVERLAY_WIDTH = 27;
    constexpr int HEADER_OVERLAY_HEIGHT = 27;
    constexpr QSize HEADER_OVERLAY_SIZE(HEADER_OVERLAY_WIDTH, HEADER_OVERLAY_HEIGHT);

    enum class HeaderType { White = 0, Magic, Rare, Unique, Gem, Currency };

    constexpr std::array<const char *, 6> FrameName{
        {"White", "Magic", "Rare", "Unique", "Gem", "Currency"}};

    constexpr std::array<const char *, 6> FrameColor{
        {"#c8c8c8", "#88f", "#ff7", "#af6025", "#1ba29b", "#aa9e82"}};

    constexpr HeaderType getHeaderType(poe::FrameType frame_type)
    {
        // clang-format off
        switch (frame_type) {
        case poe::FrameType::Normal:   return HeaderType::White;
        case poe::FrameType::Magic:    return HeaderType::Magic;
        case poe::FrameType::Rare:     return HeaderType::Rare;
        case poe::FrameType::Unique:   return HeaderType::Unique;
        case poe::FrameType::Gem:      return HeaderType::Gem;
        case poe::FrameType::Currency: return HeaderType::Currency;
        // Treat all the following frame types as white, which is what acquisition has always done.
        case poe::FrameType::DivinationCard:
        case poe::FrameType::Quest:
        case poe::FrameType::Prophecy:
        case poe::FrameType::Foil:
        case poe::FrameType::SupporterFoil:
        case poe::FrameType::Necropolis:
            return HeaderType::White;
        }
        // clang-format on
    }

} // namespace

ItemTooltip::ItemTooltip(const ItemInfo &item_info, QObject *parent)
    : QObject(parent)
{
    const bool single_line = item_info.name.isEmpty();

    // Name
    if (single_line) {
        m_name = item_info.typeLine;
        m_headerSize = HEADER_SINGLELINE_SIZE;
    } else {
        m_name = item_info.name + "\n" + item_info.typeLine;
        m_headerSize = HEADER_DOUBLELINE_SIZE;
    }
    m_headerOverlaySize = HEADER_OVERLAY_SIZE;

    const HeaderType header_type = getHeaderType(item_info.frameType);
    const auto k = std::to_underlying(header_type);
    const QString frame_name{FrameName[k]};

    m_frameColor = QString{FrameColor[k]};

    const QString suffix = single_line
                                   && ((header_type == HeaderType::Rare)
                                       || (header_type == HeaderType::Unique))
                               ? "SingleLine"
                               : "";

    const QString base_url = "qrc:/images/headers/ItemsHeader" + frame_name + suffix;

    m_headerLeft = base_url + "Left.png";
    m_headerLeftOverlay = "qrc:/images/symbols/Crusader-item-symbol.png";
    m_headerMiddle = base_url + "Middle.png";
    m_headerRight = base_url + "Right.png";

    // Icon
    m_icon = item_info.icon;

    // Background
    if (item_info.elder) {
        m_background = QString{"qrc:/images/backgrounds/ElderBackground_%1x%2.png"}
                           .arg(item_info.w)
                           .arg(item_info.h);
    } else if (item_info.shaper) {
        m_background = QString{"qrc:/images/backgrounds/ShaperBackground_%1x%2.png"}
                           .arg(item_info.w)
                           .arg(item_info.h);
    } else {
        m_background = "";
    }
}
