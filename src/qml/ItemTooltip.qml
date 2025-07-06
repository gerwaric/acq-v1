import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


import Acquisition

Item {

    visible: App.tooltip ? true : false
    enabled: App.tooltip ? true : false
    Layout.minimumWidth: 250

    ColumnLayout {

        Item {
            id: itemImage
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

        RowLayout {
            id: itemHeader
            spacing: 0

            Item {
                id: leftHeader
                Layout.preferredWidth: App.tooltip?.headerSize.width
                Layout.preferredHeight: App.tooltip?.headerSize.height

                Image {
                    anchors.fill: parent
                    source: App.tooltip?.headerLeft
                }

                Image {
                    anchors.centerIn: parent
                    width: App.tooltip?.headerOverlaySize.width
                    height: App.tooltip?.headerOverlaySize.height
                    source: App.tooltip?.headerOverlayLeft
                }
            }

            Item {
                id: middleHeader
                Layout.preferredWidth: itemName.implicitWidth
                Layout.preferredHeight: App.tooltip?.headerSize.height

                Image {
                    anchors.fill: parent
                    source: App.tooltip?.headerMiddle
                }

                Label {
                    id: itemName
                    anchors.fill: parent
                    text: App.tooltip?.name
                    color: App.tooltip?.frameColor
                    font.family: "Fontin SmallCaps"
                    font.pixelSize: 20
                    horizontalAlignment: "AlignHCenter"
                    verticalAlignment: "AlignVCenter"
                }
            }

            Item {
                id: rightHeader
                Layout.preferredWidth: App.tooltip?.headerSize.width
                Layout.preferredHeight: App.tooltip?.headerSize.height

                Image {
                    id: rightHeaderBackground
                    anchors.fill: parent
                    source: App.tooltip?.headerRight
                }

                Image {
                    id: rightHeaderOverlay
                    anchors.centerIn: parent
                    height: App.tooltip?.headerOverlaySize.height
                    width: App.tooltip?.headerOverlaySize.width
                    source: App.tooltip?.headerOverlayRight
                }
            }
        }
    }
}
