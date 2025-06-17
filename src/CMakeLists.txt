# Copyright (C) 2025 Tom Holz.
# SPDX-License-Identifier: GPL-3.0-only

# Generate version.h
configure_file(
    ${CMAKE_SOURCE_DIR}/config/app_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/app_version.h
    @ONLY
)

# Generate versioninfo.rc
configure_file(
    ${CMAKE_SOURCE_DIR}/config/app_versioninfo.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/app_versioninfo.rc
    @ONLY
)

# Add generated header directory to include path
add_library(version INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated/app_version.h)
target_include_directories(version INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated)

qt_add_executable(acquisition
    # Entry point
    main.cpp
    # Other stuff
    app.cpp
    app.h
    networkmanager.cpp
    networkmanager.h
    oauthtoken.cpp
    oauthtoken.h
    oauthmanager.cpp
    oauthmanager.h
    poeclient.cpp
    poeclient.h
    settingsmanager.cpp
    settingsmanager.h
    # Utilities
    util/glaze_qt.h
    util/json.h
    util/rfc2822.cpp
    util/rfc2822.h
    util/spdlog_qt.h
    # Databases
    datastore/datastore.cpp
    datastore/datastore.h
    datastore/globaldatastore.cpp
    datastore/globaldatastore.h
    datastore/leaguedatastore.cpp
    datastore/leaguedatastore.h
    datastore/userdatastore.cpp
    datastore/userdatastore.h
    # Path of Exile API types
    poe/account.h
    poe/accountprofile.h
    poe/character.h
    poe/cruciblenode.h
    poe/displaymode.h
    poe/eventladderentry.h
    poe/frametype.h
    poe/guild.h
    poe/item.h
    poe/itemfilter.h
    poe/itemjeweldata.h
    poe/itemproperty.h
    poe/itemsocket.h
    poe/ladderentry.h
    poe/league.h
    poe/leagueaccount.h
    poe/leaguerule.h
    poe/passivegroup.h
    poe/passivenode.h
    poe/publicstashchange.h
    poe/pvpladderteamentry.h
    poe/pvpladderteammember.h
    poe/pvpmatch.h
    poe/stashtab.h
    poe/webcharacter.h
    poe/webstashtab.h
    # API rate limiting
    ratelimit/ratelimit.cpp
    ratelimit/ratelimit.h
    ratelimit/ratelimitdialog.cpp
    ratelimit/ratelimitdialog.h
    ratelimit/ratelimitedreply.cpp
    ratelimit/ratelimitedreply.h
    ratelimit/ratelimitedrequest.cpp
    ratelimit/ratelimitedrequest.h
    ratelimit/ratelimiter.cpp
    ratelimit/ratelimiter.h
    ratelimit/ratelimitmanager.cpp
    ratelimit/ratelimitmanager.h
    ratelimit/ratelimitpolicy.cpp
    ratelimit/ratelimitpolicy.h
)

target_include_directories(acquisition PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(acquisition
    PRIVATE
    # Qt Libraries
    Qt::Core
    Qt::Gui
    Qt::NetworkAuth
    Qt::Qml
    Qt::Quick
    Qt::Sql
    # External Libraries
    boost-headers-only
    glaze::glaze
    spdlog::spdlog
    # Internal Libraries
    version
)

set_target_properties(acquisition PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER app.acquisition.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
