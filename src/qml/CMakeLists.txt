# Copyright (C) 2025 Tom Holz.
# SPDX-License-Identifier: GPL-3.0-only

qt_add_qml_module(acquisition
    URI "Acquisition"
    OUTPUT_DIRECTORY "Acquisition"
    QML_FILES
        CharactersTab.qml
        ItemsTab.qml
        ItemTooltip.qml
        Main.qml
        MainTabs.qml
        SettingsTab.qml
        StashesTab.qml
)
