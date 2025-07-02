// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition
//import Acquisition.qml

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

    MainTabs {}
}
