# Copyright (C) 2025 Tom Holz.
# SPDX-License-Identifier: GPL-3.0-only

# Generate version.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/app_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/app_version.h
    @ONLY
)

# Generate versioninfo.rc
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/app_versioninfo.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/app_versioninfo.rc
    @ONLY
)

# Add generated header directory to include path
if(WIN32)
    add_library(acquisition_config INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/generated/app_version.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/app_versioninfo.rc
    )
else()
    add_library(acquisition_config INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/generated/app_version.h
    )
endif()

target_include_directories(acquisition_config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated)
