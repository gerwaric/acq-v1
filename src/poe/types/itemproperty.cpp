// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "itemproperty.h"

#include "util/spdlog_qt.h"

static_assert(ACQUISITION_USE_SPDLOG);

namespace {

    QString renderValue(const std::tuple<QString, int> &pair)
    {
        static const QStringList Colors{
            "#ffffff", // white
            "#8888ff", // light blue
            "#d20000", // bright red
            "#ffffff", // white (again)
            "#960000", // dark red / blood red
            "#366492", // steel blue
            "gold",    // gold (HTML name)
            "#d02090"  // violet red / deep pink
        };
        const auto &[value, type] = pair;
        const size_t k = ((type >= 0) && (type < Colors.size())) ? type : 0;
        return "<font color='" + Colors[k] + "'>" + value + "</font>";
    }

    QString renderNameFirst(const poe::ItemProperty &property)
    {
        const QString &name = property.name;
        if (property.values.empty()) {
            return name;
        }
        if (property.values.size() > 1) {
            spdlog::error("Cannot render name-first item property '{}': too many values", name);
            return "";
        }
        QString result = name + " " + renderValue(property.values[0]);
        if (property.suffix) {
            result += " " + property.suffix.value();
        }
        return result;
    }

    QString renderValueFirst(const poe::ItemProperty &property)
    {
        const QString &name = property.name;
        if (property.values.empty()) {
            spdlog::error("Cannot render value-first item property '{}': values is empty", name);
            return "";
        }
        if (property.values.size() > 1) {
            spdlog::error("Cannot render value-first item property '{}': more than one value", name);
            return "";
        }
        QString result = renderValue(property.values[0]) + " " + name;
        if (property.suffix) {
            result += " " + property.suffix.value();
        }
        return result;
    }

    QString renderProgress(const poe::ItemProperty &property)
    {
        const QString &name = property.name;
        if (property.values.empty()) {
            spdlog::error("Cannot render progress bar item property '{}': values is empty", name);
            return "";
        }
        if (property.values.size() > 1) {
            spdlog::error("Cannot render progress bar item property '{}': more than one value",
                          name);
            return "";
        }
        QString result = renderValue(property.values[0]);
        if (property.suffix) {
            result += " " + property.suffix.value();
        }
        return result;
    }

    QString renderInsertedValues(const poe::ItemProperty &property)
    {
        QString result{property.name};
        for (auto i = 0; i < property.values.size(); ++i) {
            const QString tag = "{" + QString::number(i) + "}";
            const QString value = renderValue(property.values[i]);
            result.replace(tag, value);
        }
        if (property.suffix) {
            result += " " + property.suffix.value();
        }
        return result;
    }

} // namespace

namespace poe {

    QString ItemProperty::render() const
    {
        if (!displayMode) {
            return "";
        }

        // clang-format off
        switch (displayMode.value()) {
        case DisplayMode::NameFirst:      return renderNameFirst(*this);
        case DisplayMode::ValuesFirst:    return renderValueFirst(*this);
        case DisplayMode::ProgressBar:    return renderProgress(*this);
        case DisplayMode::InsertedValues: return renderInsertedValues(*this);
        case DisplayMode::Separator:
            spdlog::error("Item property asked to render a separator. This should not happen.");
            return "";
        }
        // clang-format on
    }

} // namespace poe
