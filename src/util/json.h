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

    // Local helper functions that allow us to expose the strict vs
    // permissive choice at runtime.

    template<typename T>
    bool parse_permissive(T &out, std::string_view json)
    {
        constexpr glz::opts options{.error_on_unknown_keys = false};
        glz::context ctx;
        const glz::error_ctx error = glz::read<options>(out, json, ctx);
        if (error) {
            const std::string type_name = typeid(T).name();
            const std::string error_message = glz::format_error(error, json);
            spdlog::error("Error parsing json (permissive) to {}: {}", type_name, error_message);
        };
        return !error;
    }

    template<typename T>
    bool parse_strict(T &out, std::string_view json)
    {
        const glz::error_ctx error = glz::read_json(out, json);
        if (error) {
            const std::string type_name = typeid(T).name();
            const std::string error_message = glz::format_error(error, json);
            spdlog::error("Error parsing json (strict) to {}: {}", type_name, error_message);
        };
        return !error;
    }
} // namespace

namespace json {

    enum ParseMode { STRICT, PERMISSIVE };

    // Serialization

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
        };
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

    // Deserialization into an argument.

    template<typename T>
    bool parse_into(T &out, std::string_view json, ParseMode mode)
    {
        switch (mode) {
        case ParseMode::STRICT:
            return parse_strict(out, json);
        case ParseMode::PERMISSIVE:
            return parse_permissive(out, json);
        };
    }

    template<typename T>
    bool parse_into(T &out, QByteArrayView json, ParseMode mode)
    {
        const std::string_view sv(json.constData(), json.size());
        return parse_into(out, sv, mode);
    }

    template<typename T>
    bool parse_into(T &out, QStringView json, ParseMode mode)
    {
        const QByteArray utf8 = json.toUtf8();
        const std::string_view sv(utf8.constData(), utf8.size());
        return parse_into(out, sv, mode);
    }

    // Deserialization into a return value.

    template<typename T>
    std::pair<T, bool> from_json(std::string_view json, ParseMode mode)
    {
        T out;
        const bool ok = json::parse_into<T>(out, json, mode);
        return {std::move(out), ok};
    }

    template<typename T>
    std::pair<T, bool> from_json(QByteArrayView json, ParseMode mode)
    {
        T out;
        const bool ok = json::parse_into<T>(out, json, mode);
        return {std::move(out), ok};
    }

    template<typename T>
    std::pair<T, bool> from_json(QStringView json, ParseMode mode)
    {
        T out;
        const bool ok = json::parse_into<T>(out, json, mode);
        return {std::move(out), ok};
    }
} // namespace json
