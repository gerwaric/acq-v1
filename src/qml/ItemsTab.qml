import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition

Item {
    id: itemsTab

    RowLayout {
        anchors.fill: parent

        GridLayout {
            Layout.minimumWidth: 200
            columns: 2

            Label { text: "Realm" }
            ComboBox {
                id: realm
                model: App.getRealmNames()
                onCurrentTextChanged: {
                    league.model = App.getLeagueNames(currentText)
                }
            }

            Label { text: "League" }
            ComboBox {
                id: league
            }

            Button {
                text: "Load Items"
                onClicked: {
                    App.loadItems(realm.currentText, league.currentText)
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.columnSpan: 2
            }

        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.minimumWidth: 200

            HorizontalHeaderView {
                id: itemsHeader
                Layout.fillWidth: true

                clip: true
                syncView: itemsView
            }

            TreeView {
                id: itemsView
                Layout.fillHeight: true
                Layout.fillWidth: true

                clip: true
                boundsMovement: Flickable.StopAtBounds

                model: App.itemsModel
                selectionModel: App.itemSelectionModel

                delegate: TreeViewDelegate {}

                ScrollBar.horizontal: ScrollBar {}
                ScrollBar.vertical:   ScrollBar {}
            }
        }

        ItemTooltip {}
    }
}
