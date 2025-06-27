// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition
import Acquisition.qml

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "Acquisition - v" + App.version

    Settings {
        id: settings
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
        property alias logLevel: logLevel.currentIndex
        property alias tabIndex: tabs.currentIndex
    }

    footer: RowLayout {
        width: parent.width
        spacing: 10

        Label {
            text: "User:"
        }

        Button {
            text: App.isAuthenticated ? App.username : "Authenticate"
            onClicked: App.authenticate()
        }

        Label {
            text: "Realm:"
        }

        ComboBox {
            id: realm
            model: App.getRealmNames()
        }

        Label {
            text: "Log Level:"
        }
        ComboBox {
            id: logLevel
            model: ["off","trace","debug","info","warn","error","critical"]
            onCurrentTextChanged: {
                App.setLogLevel(currentText)
            }
        }

        Label {
            Layout.fillWidth: true
            text: App.rateLimitStatus
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        enabled: App.isAuthenticated

        TabBar {
            id: tabs
            Layout.fillWidth: parent
            //TabButton { padding: 10; text: "Settings" }
            TabButton { padding: 10; text: "Characters" }
            TabButton { padding: 10; text: "Stash Tabs" }
            TabButton { padding: 10; text: "Items" }
        }

        StackLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: tabs.currentIndex

            /*
            SettingsTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            */

            CharactersTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            StashesTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            ItemsTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

        }
    }
}
