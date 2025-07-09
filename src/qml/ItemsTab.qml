import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition

Item {
    id: itemsTab

    SplitView {
        anchors.fill: parent

        GridLayout {
            SplitView.minimumWidth: 200
            SplitView.fillHeight: true
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
            SplitView.fillHeight: true
            SplitView.minimumWidth: 200

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

        ColumnLayout {
            visible: App.tooltip ? true : false
            enabled: App.tooltip ? true : false
            SplitView.minimumWidth: 400

            Item {
                id: itemImage
                //Layout.width: itemIcon.implicitWidth
                //Layout.height: itemIcon.implicitHeight
                Layout.preferredWidth: itemIcon.implicitWidth
                Layout.preferredHeight: itemIcon.implicitHeight

                Image {
                    anchors.centerIn: parent
                    source: App.tooltip?.background
                }

                Image {
                    id: itemIcon
                    anchors.fill: parent
                    source: App.tooltip?.icon
                }
            }

            ItemTooltip {}

            Item {
                id: tooltipSpacer
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
