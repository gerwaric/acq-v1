// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "itemtooltip.h"

#include "poe/types/frametype.h"

#include <QDir>
#include <QImage>
#include <QResource>
#include <QStringLiteral>
#include <QStringView>

namespace {

    constexpr const char *IMPLICIT_MODS_COLOR = "#8888ff";
    constexpr const char *ENCHANTED_MODS_COLOR = "#b4b4ff";
    constexpr const char *FRACTURED_MODS_COLOR = "#a29162;";
    constexpr const char *EXPLICIT_MODS_COLOR = "#8888ff";
    constexpr const char *CRAFTED_MODS_COLOR = "#b4b4ff";
    constexpr const char *RED_MODS = "#d20000";

    constexpr int HEADER_SINGLELINE_WIDTH = 29;
    constexpr int HEADER_SINGLELINE_HEIGHT = 34;
    constexpr QSize HEADER_SINGLELINE_SIZE(HEADER_SINGLELINE_WIDTH, HEADER_SINGLELINE_HEIGHT);

    constexpr int HEADER_DOUBLELINE_WIDTH = 44;
    constexpr int HEADER_DOUBLELINE_HEIGHT = 54;
    constexpr QSize HEADER_DOUBLELINE_SIZE(HEADER_DOUBLELINE_WIDTH, HEADER_DOUBLELINE_HEIGHT);

    constexpr int HEADER_OVERLAY_WIDTH = 27;
    constexpr int HEADER_OVERLAY_HEIGHT = 27;
    constexpr QSize HEADER_OVERLAY_SIZE(HEADER_OVERLAY_WIDTH, HEADER_OVERLAY_HEIGHT);

    enum class HeaderType { White = 0, Magic, Rare, Unique, Gem, Currency, Quest };

    constexpr std::array<const char *, 7> FrameName{
        {"White", "Magic", "Rare", "Unique", "Gem", "Currency", "Quest"}};

    constexpr std::array<const char *, 7> FrameColor{
        {"#c8c8c8", "#88f", "#ff7", "#af6025", "#1ba29b", "#aa9e82", "#c8c8c8"}};

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
        default:                       return HeaderType::White;
        }
        // clang-format on
    }

} // namespace

ItemTooltip::ItemTooltip(const ItemData &item, QObject *parent)
    : QObject(parent)
{
    const bool single_line = item.name.isEmpty();

    // Name
    if (single_line) {
        m_name = item.typeLine;
        m_headerSize = HEADER_SINGLELINE_SIZE;
    } else {
        m_name = item.name + "\n" + item.typeLine;
        m_headerSize = HEADER_DOUBLELINE_SIZE;
    }
    m_headerOverlaySize = HEADER_OVERLAY_SIZE;

    // We have to handle the quest type differently since there's not separate
    // header images for it.
    const HeaderType header_type = getHeaderType(item.frameType);
    const auto k = std::to_underlying(header_type);
    const QString frame_name{FrameName[k]};

    m_frameColor = QString{FrameColor[k]};

    const QString suffix = single_line
                                   && ((header_type == HeaderType::Rare)
                                       || (header_type == HeaderType::Unique))
                               ? "SingleLine"
                               : "";

    const QString base_url = "qrc:/images/headers/ItemsHeader"
                             + (frame_name == "Quest" ? "White" : frame_name) + suffix;

    m_headerLeft = base_url + "Left.png";
    m_headerMiddle = base_url + "Middle.png";
    m_headerRight = base_url + "Right.png";

    m_headerLeftOverlay = "qrc:/images/symbols/Crusader-item-symbol.png";
    m_headerRightOverlay = "qrc:/images/symbols/Crusader-item-symbol.png";

    // Icon
    m_icon = item.icon;

    // Background
    if (item.elder) {
        m_background = QString{"qrc:/images/backgrounds/ElderBackground_%1x%2.png"}
                           .arg(item.w)
                           .arg(item.h);
    } else if (item.shaper) {
        m_background = QString{"qrc:/images/backgrounds/ShaperBackground_%1x%2.png"}
                           .arg(item.w)
                           .arg(item.h);
    } else {
        m_background = "";
    }

    m_separatorUrl = "qrc:/images/separators/Separator" + frame_name + ".png";
    m_propertiesText1 = item.propertiesText1;
    m_propertiesText2 = item.propertiesText2;
    m_requirementsTest = item.requirementsText;

    // Implict modifiers get their own section.
    if (!item.implicitMods.isEmpty()) {
        m_implicitsText = renderModList(item.implicitMods, IMPLICIT_MODS_COLOR);
    }

    // Enchantments get their own section.
    if (!item.enchantMods.isEmpty()) {
        m_enchantmentsText = renderModList(item.enchantMods, ENCHANTED_MODS_COLOR);
    }

    // Fractured, explicit, and crafted modifiers appear in the same section.
    QStringList modifiers;
    if (!item.fracturedMods.isEmpty()) {
        modifiers.append(renderModList(item.fracturedMods, FRACTURED_MODS_COLOR));
    }
    if (!item.explicitMods.isEmpty()) {
        modifiers.append(renderModList(item.explicitMods, EXPLICIT_MODS_COLOR));
    }
    if (!item.craftedMods.isEmpty()) {
        modifiers.append(renderModList(item.craftedMods, CRAFTED_MODS_COLOR));
    }
    m_modifiersText = modifiers.join("<br>");

    QStringList sections;
    if (!m_propertiesText1.isEmpty()) {
        sections.append(m_propertiesText1);
    }
    if (!m_propertiesText2.isEmpty()) {
        sections.append(m_propertiesText1);
    }
    if (!m_requirementsTest.isEmpty()) {
        sections.append(m_requirementsTest);
    }
    if (!m_implicitsText.isEmpty()) {
        sections.append(m_implicitsText);
    }
    if (!m_enchantmentsText.isEmpty()) {
        sections.append(m_enchantmentsText);
    }
    if (!m_modifiersText.isEmpty()) {
        sections.append(m_modifiersText);
    }

    // Corrupted and unidentified get their own section
    QStringList misc;
    if (!item.identified) {
        const poe::ItemProperty property{.values{{"Unidentified", 2}},
                                         .displayMode = poe::DisplayMode::NameFirst};
        misc.append(property.render());
    }
    if (item.corrupted) {
        const poe::ItemProperty property{.values{{"Corrupted", 2}},
                                         .displayMode = poe::DisplayMode::NameFirst};
        misc.append("Corrupted");
    }
    if (!misc.isEmpty()) {
        sections.append(renderModList(misc, RED_MODS));
    }

    m_tooltipSections = sections;
}

/*
QString ItemTooltip::renderDisplayText(const ItemInfo &item)
{
    QStringList sections = item.displayTextSections;
    if (!item.implicitMods.isEmpty()) {
        sections.append(renderModList(item.implicitMods, IMPLICIT_MODS_COLOR));
    }
    if (!item.enchantMods.isEmpty()) {
        sections.append(renderModList(item.enchantMods, ENCHANTED_MODS_COLOR));
    }

    QStringList modList;
    if (!item.fracturedMods.isEmpty()) {
        modList.append(renderModList(item.fracturedMods, FRACTURED_MODS_COLOR));
    }
    if (!item.explicitMods.isEmpty()) {
        modList.append(renderModList(item.explicitMods, EXPLICIT_MODS_COLOR));
    }
    if (!item.craftedMods.isEmpty()) {
        modList.append(renderModList(item.craftedMods, CRAFTED_MODS_COLOR));
    }
    sections.append(modList.join("<br>"));
    const QString body = sections.join(separator).replace("\n", "<br>");
    return "<div style=\"text-align:center; line-height:1; margin: auto;\">" + body + "</div>";
}*/

QString ItemTooltip::renderModList(const QStringList &mods, const char *color)
{
    const QString joined = mods.join("<br>").replace("\n", "<br>");
    return "<font color='" + QString(color) + "'>" + joined + "</font>";
}
