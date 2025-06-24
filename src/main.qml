// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "Acquisition - v" + App.version

    footer: RowLayout {
        Layout.fillWidth: parent
        width: parent.width
        spacing: 10
        Label {
            text: App.rateLimitStatus
        }
    }

    ColumnLayout {
        width: parent.width

        TabBar {
            id: tabs
            Layout.fillWidth: parent
            TabButton { padding: 10; text: "Settings" }
            TabButton { padding: 10; text: "Characters" }
            TabButton { padding: 10; text: "Stash Tabs" }
            TabButton { padding: 10; text: "Items" }
        }

        StackLayout {
            Layout.margins: 20
            Layout.fillWidth: parent
            currentIndex: tabs.currentIndex

            GridLayout {
                id: settingsTab
                columns: 2

                // Logging row
                Label { text: "Logging Level:" }
                ComboBox {
                    id: currentLogLevel
                    model: ["off","trace","debug","info","warn","error","critical"]
                    onCurrentTextChanged: App.debugLevel = currentText
                }

                // Realm Row
                Label { text: "Realm:"}
                ComboBox {
                    id: currentRealm
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
                    onClicked: App.poeClient.listLeagues(currentRealm.currentText)
                }
                ComboBox {
                    id: currentLeague
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
                    onClicked: App.poeClient.listCharacters(currentRealm.currentText)
                }
                ComboBox {
                    id: currentCharacter
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
                    onClicked: App.poeClient.listStashes(currentRealm.currentText, currentLeague.currentText)
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

            ColumnLayout {
                id: characterTab
                Layout.fillWidth: parent
                Layout.fillHeight: parent

                HorizontalHeaderView {
                    id: characterHeader
                    clip: true
                    syncView: characterView
                    Layout.fillWidth: true
                }

                TableView {
                    id: characterView
                    Layout.fillWidth: parent
                    Layout.fillHeight: parent
                    columnSpacing: 2
                    rowSpacing: 2
                    clip: true

                    model: App.characterModel

                    delegate: TableViewDelegate {
                        text: modelData
                        implicitWidth: 100
                    }
                }
            }

            ColumnLayout {
                id: stashTab
                Layout.fillWidth: parent
                Layout.fillHeight: parent

                HorizontalHeaderView {
                    id: stashHeader
                    clip: true
                    syncView: stashView
                    Layout.fillWidth: true
                }

                TableView {
                    id: stashView
                    Layout.fillWidth: parent
                    Layout.fillHeight: parent
                    columnSpacing: 2
                    rowSpacing: 2
                    clip: true

                    model: App.stashModel

                    delegate: TableViewDelegate {
                        text: modelData
                        implicitWidth: 100
                    }
                }
            }


            ColumnLayout {
                id: itemsTab
                //SplitView.fillWidth: true

                HorizontalHeaderView {
                    id: itemsHeader
                    clip: true
                    syncView: itemsView
                    Layout.fillWidth: true
                }

                TreeView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    id: itemsView
                    clip: true
                    boundsMovement: Flickable.StopAtBounds

                    model: App.itemsModel

                    delegate: TreeViewDelegate {}

                    ScrollBar.horizontal: ScrollBar {}
                    ScrollBar.vertical:   ScrollBar {}

               }

            }
        }
    }
}
