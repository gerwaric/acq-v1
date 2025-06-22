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

qt_add_executable(acquisition WIN32 MACOSX_BUNDLE
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
    poe.h
    poeclient.cpp
    poeclient.h
    settingsmanager.cpp
    settingsmanager.h
    # Models
    models/itemrow.cpp
    models/itemrow.h
    # Item Model
    model/characterinfo.cpp
    model/characterinfo.h
    model/characternode.cpp
    model/characternode.h
    model/iteminfo.cpp
    model/iteminfo.h
    model/itemnode.cpp
    model/itemnode.h
    model/rootnode.cpp
    model/rootnode.h
    model/stashinfo.cpp
    model/stashinfo.h
    model/stashnode.cpp
    model/stashnode.h
    model/treemodel.cpp
    model/treemodel.h
    model/treenode.cpp
    model/treenode.h
    # Utilities
    util/glaze_qt.h
    util/json.h
    util/rfc2822.cpp
    util/rfc2822.h
    util/spdlog_qt.h
    # Databases
    datastore/datastore.cpp
    datastore/datastore.h
    datastore/globalstore.cpp
    datastore/globalstore.h
    datastore/userstore.cpp
    datastore/userstore.h
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

qt_add_qml_module(acquisition
    URI "Acquisition"
    QML_FILES
        main.qml
        qml/MainControlPanel.qml
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

if(APPLE)
    set_target_properties(acquisition PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER app.acquisition.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()
