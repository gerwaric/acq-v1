// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "util/glaze_qt.h"

#include <spdlog/spdlog.h>

#include <QByteArrayView>
#include <QStringView>

#include <string>
#include <string_view>

namespace {

    // --- Layer 1: Low-level implementation ---
    template<auto Opts, typename T>
    bool parse_into_impl(T &out, std::string_view json)
    {
        glz::context ctx{};
        const auto err = glz::read<Opts>(out, json, ctx);
        if (err) {
            const std::string type_name = typeid(T).name();
            const std::string error_message = glz::format_error(err, json);
            spdlog::error("json error parsing {}: {}", type_name, error_message);
        }
        return !err;
    }

    template<auto Opts, typename T>
    std::pair<T, bool> parse_impl(std::string_view json)
    {
        T out{};
        bool ok = parse_into_impl<Opts>(out, json);
        return {out, ok};
    }

    // --- Layer 2: Adapter overloads ---

    // Adapter for std::string
    template<auto Opts, typename T>
    std::pair<T, bool> parse(std::string_view json)
    {
        return parse_impl<Opts, T>(json);
    }

    // Adapter for QByteArray
    template<auto Opts, typename T>
    std::pair<T, bool> parse(QByteArrayView json)
    {
        return parse_impl<Opts, T>(std::string_view{json});
    }

    // Adapter for QString
    template<auto Opts, typename T>
    std::pair<T, bool> parse(QStringView json)
    {
        // Must convert from UTF-16 to UTF-8
        const QByteArray utf8 = json.toUtf8();
        return parse_impl<Opts, T>(std::string_view{utf8});
    }

} // namespace

namespace json {

    template<typename T>
    std::pair<T, bool> parse_strict(auto &&input)
    {
        static constexpr glz::opts strict_opts{.error_on_unknown_keys = true};
        return parse<strict_opts, T>(std::forward<decltype(input)>(input));
    }

    template<typename T>
    std::pair<T, bool> parse_permissive(auto &&input)
    {
        static constexpr glz::opts permissive_opts{.error_on_unknown_keys = false};
        return parse<permissive_opts, T>(std::forward<decltype(input)>(input));
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
