// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: "Acquisition " + App.version

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 8

        Button {
            text: App.isAuthenticated ? "Authenticated" : "Authenticate"
            onClicked: App.authenticate()
        }

        Label {
            text: App.isAuthenticated ? "Logged in as " + App.username : "Not logged in"
        }

        Button {
            text: "Get leagues"
            enabled: App.isAuthenticated
        }

        Label {
            text: "Leagues:"
        }

        ComboBox {

        }

        Button {
            text: "List Characters"
            enabled: App.isAuthenticated
        }

        Label {
            text: "Characters:"
        }

        ComboBox {

        }
    }
}
