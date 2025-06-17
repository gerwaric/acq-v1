// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QString>

struct OAuthToken
{
    QString access_token;
    long long expires_in{0};
    QString refresh_token;
    QString scope;
    QString sub;
    QString token_type;
    QString username;
};
