// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "util/glaze_qt.h"

static_assert(ACQUISITION_USE_GLAZE); // Avoids Qt Creator warnings about unused headers.

#include <spdlog/spdlog.h>

#include <QByteArrayView>
#include <QStringView>

#include <string>
#include <string_view>

namespace {

    template<typename T, auto GlazeOpts>
    bool parse_into_impl(T &output, const QByteArray &data)
    {
        const std::string_view str(data.constData(), data.size());
        const auto err = glz::read<GlazeOpts>(output, str);
        if (err) {
            const std::string type_name = typeid(T).name();
            const std::string error_message = glz::format_error(err, str);
            spdlog::error("json: glaze error parsing {}: {}", type_name, error_message);
            return false;
        }
        return true;
    };

} // namespace

namespace json {

    enum class Mode { Strict, Permissive };

    template<typename T>
    bool parse_into(T &output, const QByteArray &data, Mode mode)
    {
        switch (mode) {
        case Mode::Strict:
            return parse_into_impl<T, glz::opts{.error_on_unknown_keys = true}>(output, data);
        case Mode::Permissive:
            return parse_into_impl<T, glz::opts{.error_on_unknown_keys = false}>(output, data);
        }
    }

    template<typename T>
    std::pair<T, bool> parse(const QByteArray &data, Mode mode)
    {
        T output;
        const bool ok = parse_into(output, data, mode);
        return {output, ok};
    }

    template<typename T>
    std::string toStdString(const T &value)
    {
        std::string buffer;
        const auto result = glz::write_json(value, buffer);
        if (result) {
            const std::string type_name = typeid(T).name();
            const std::string error_message = glz::format_error(result, buffer);
            spdlog::error("Error serializing {} into json: {}", type_name, error_message);
            buffer = "";
        }
        return buffer;
    }

    template<typename T>
    inline QByteArray toByteArray(const T &value)
    {
        const std::string json = json::toStdString(value);
        return QByteArray::fromStdString(json);
    }

    template<typename T>
    inline QString toString(const T &value)
    {
        const QByteArray utf8 = json::toByteArray(value);
        return QString::fromUtf8(utf8);
    }

} // namespace json
