// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

#include "ratelimitedrequest.h"

// Total number of rate-limited requests that have been created.
unsigned long RateLimitedRequest::s_request_count = 0;
