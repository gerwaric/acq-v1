// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <spdlog/spdlog.h>

// For formatting basic Qt types.
#include <QByteArray>
#include <QByteArrayView>
#include <QString>
#include <QStringView>
#include <QVariant>

// For formatting Qt enums (add more if needed).
#include <QAbstractOAuth>
#include <QMetaEnum>
#include <QNetworkReply>

// This is a helper define to avoid Qt Creator's warnings that this header is unused.
#define ACQUISITION_USE_SPDLOG

// Define an inline helper function to convert levels into QStrings.

inline QString toString(spdlog::level::level_enum level)
{
    const auto sv = spdlog::level::to_string_view(level);
    return QString::fromUtf8(sv.data(), sv.size());
}

// Define some helper classes in a local namespace.

namespace {

    // Base formatter class for various custom formatters.
    struct BaseFormatter
    {
        constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin())
        {
            return ctx.end();
        }
    };

    // Use a base class for Qt byte array formatters.
    template<typename ByteArrayType>
    struct ByteArrayFormatter : public BaseFormatter
    {
        auto format(const ByteArrayType &arr, fmt::format_context &ctx) const -> decltype(ctx.out())
        {
            const auto sv = std::string_view(arr.constData(), arr.size());
            return fmt::format_to(ctx.out(), "{}", sv);
        }
    };

    // Use a base class for Qt string formatters.
    template<typename StringType>
    struct StringFormatter : public BaseFormatter
    {
        auto format(const StringType &str, fmt::format_context &ctx) const -> decltype(ctx.out())
        {
            const auto utf8 = str.toUtf8();
            const auto sv = std::string_view(utf8.constData(), utf8.size());
            return fmt::format_to(ctx.out(), "{}", sv);
        }
    };

    // Use a base class for Qt Enum formatters to subclass.
    template<typename T>
    struct EnumFormatter : public BaseFormatter
    {
        static_assert(std::is_enum<T>::value, "T must be an enum type");
        template<typename FormatContext>
        auto format(const T &value, FormatContext &ctx) const -> decltype(ctx.out())
        {
            const QMetaEnum metaEnum = QMetaEnum::fromType<T>();
            const char *key = metaEnum.valueToKey(static_cast<int>(value));
            if (key) {
                return fmt::format_to(ctx.out(), "{}", key);
            } else {
                return fmt::format_to(ctx.out(), "{}({})", metaEnum.name(), static_cast<int>(value));
            }
        }
    };
} // namespace

// Now create formatters for various Qt classes and enums.

namespace fmt {

    // Formatter for spdlog::level.
    template<>
    struct formatter<spdlog::level::level_enum> : BaseFormatter
    {
        auto format(const spdlog::level::level_enum &level, fmt::format_context &ctx) const
            -> decltype(ctx.out())
        {
            return fmt::format_to(ctx.out(), "{}", spdlog::level::to_string_view(level));
        }
    };

    // String and byte array formatters.
    template<>
    struct formatter<QString> : StringFormatter<QString>
    {};
    template<>
    struct formatter<QStringView> : StringFormatter<QStringView>
    {};
    template<>
    struct formatter<QByteArray> : ByteArrayFormatter<QByteArray>
    {};
    template<>
    struct formatter<QByteArrayView> : ByteArrayFormatter<QByteArrayView>
    {};

    // Enum formatters.
    template<>
    struct formatter<QNetworkReply::NetworkError, char> : EnumFormatter<QNetworkReply::NetworkError>
    {};
    template<>
    struct formatter<Qt::CheckState, char> : EnumFormatter<Qt::CheckState>
    {};

    // QVariant formatter.
    template<>
    struct formatter<QVariant>
    {
        constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin())
        {
            return ctx.end();
        }
        auto format(const QVariant &var, fmt::format_context &ctx) const -> decltype(ctx.out())
        {
            if (!var.isValid()) {
                return fmt::format_to(ctx.out(), "<invalid>");
            }
            if (var.isNull()) {
                return fmt::format_to(ctx.out(), "<null>");
            }
            const auto utf8 = var.toString().toUtf8();
            const auto sv = std::string_view(utf8.constData(), utf8.size());
            return fmt::format_to(ctx.out(), "{}", sv);
        }
    };
} // namespace fmt
