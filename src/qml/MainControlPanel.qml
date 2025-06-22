// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition

Frame {

    GridLayout {
        anchors.fill: parent
        columns: 2

        // Logging row
        Label { text: "Logging Level:" }
        ComboBox {
            model: ["off","trace","debug","info","warn","error","critical"]
            onCurrentTextChanged: App.debugLevel = currentText
        }

        // Realm Row
        Label { text: "Realm:"}
        ComboBox {
            model: ["pc","poe2","xbox","sony"]
            onCurrentTextChanged: App.realm = currentText
        }

        // Authentication Row
        Label { text: App.isAuthenticated ? "Logged in as " + App.username : "Not logged in"}
        Button {
            text: App.isAuthenticated ? "Re-Authenticate" : "Authenticate"
            onClicked: App.authenticate()
        }

        // League List Row
        Button {
            text: "List leagues"
            enabled: App.isAuthenticated
            onClicked: App.listLeagues()
        }
        ComboBox {
            model: App.leagues
            enabled: App.isAuthenticated
            onCurrentTextChanged: {
                App.league = currentText
            }
        }

        // Character List Row
        Button {
            text: "List Characters"
            enabled: App.isAuthenticated
            onClicked: App.listCharacters()
        }
        ComboBox {
            model: App.characters
            enabled: App.isAuthenticated
            onCurrentTextChanged: {
                App.character = currentText
            }
        }

        // Stash List Row

        Button {
            text: "List Stashes"
            enabled: App.isAuthenticated
            onClicked: App.listStashes()
        }
        ComboBox {
            model: App.stashes
            enabled: App.isAuthenticated
            onCurrentTextChanged: {
                App.stash = currentText
            }
        }

        // Actions

        Button {
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            text: "Fetch Character from GGG"
            enabled: App.isAuthenticated
            onClicked: App.getAllCharacters()
        }

        Button {
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            text: "Fetch Stashes from GGG"
            enabled: App.isAuthenticated
            onClicked: App.getAllStashes()
        }

        Button {
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            text: "Load Characters from database"
            enabled: App.isAuthenticated
            onClicked: App.loadSelectedCharacters()
        }

        Button {
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            text: "Load Stashes from database"
            enabled: App.isAuthenticated
            onClicked: App.loadSelectedStashes()
        }
    }
}
