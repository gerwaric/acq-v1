# Copyright (C) 2025 Tom Holz.
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.24)

project(acquisition
    LANGUAGES CXX
    VERSION 1.0
    DESCRIPTION "Stash management and forum shop automation for Path of Exile 1 & 2"
    HOMEPAGE_URL "https://github.com/gerwaric/acquisition-v1"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use the version tagfor suffixes like "-beta.1"
set(APP_VERSION_TAG "-alpha.1") 

# Set version variables.
set(APP_NAME            ${PROJECT_NAME})
set(APP_PUBLISHER       "GERWARIC")
set(APP_PUBLISHER_EMAIL "gerwaric@gmail.com")
set(APP_COPYRIGHT       "Copyright (C) 2025 Tom Holz")
set(APP_TRADEMARK       "Path of Exile is a trademark of Grinding Gear Games, Ltd.")
set(APP_VERSION_STRING  "${PROJECT_VERSION}${APP_VERSION_TAG}")

# Ensure all app version components are set
foreach(part MAJOR MINOR PATCH TWEAK)
    if("${PROJECT_VERSION_${part}}" STREQUAL "")
        set(APP_VERSION_${part} 0)
    else()
        set(APP_VERSION_${part} ${PROJECT_VERSION_${part}})
    endif()
endforeach()

include(cmake/PreventInSourceBuild.cmake)
include(cmake/CppWarningFlags.cmake)
#include(cmake/FetchContentHelpers.cmake)

include(cmake/Dependencies.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Gui NetworkAuth Qml Quick Sql)

qt_standard_project_setup(REQUIRES 6.9)

add_subdirectory(qml)
add_subdirectory(src)

#enable_testing()
#add_subdirectory(libs/tests)
