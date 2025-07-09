import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


import Acquisition

Item {
    id: root
    width: tooltipLayout.width
    height: tooltipLayout.height

    property int tooltipWidth: Math.max(itemHeader.preferredWidth, tooltipColumn.preferredWidth)

    ColumnLayout {
        id: tooltipLayout
        spacing: 0

        RowLayout {
            id: itemHeader
            spacing: 0
            Layout.preferredWidth: root.tooltipWidth
            Layout.preferredHeight: App.tooltip?.headerSize.height

            Item {
                id: leftHeader
                Layout.preferredWidth: App.tooltip?.headerSize.width
                Layout.fillHeight: true

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
                Layout.fillWidth: true
                Layout.fillHeight: true

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
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item {
                id: rightHeader
                Layout.preferredWidth: App.tooltip?.headerSize.width
                Layout.fillHeight: true

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

        ColumnLayout {
            id: tooltipColumn
            spacing: 0

            Rectangle {
                color: "black"
                Layout.preferredHeight: 5
                Layout.fillWidth: true
            }

            Repeater {
                id: sectionRepeater
                model: (App.tooltip?.tooltipSections.length * 2) - 1

                delegate: Loader {
                    Layout.fillWidth: true
                    sourceComponent: ((index % 2) === 0) ? labelComponent : separatorComponent
                    property int sectionIndex: index / 2
                }

                Component {
                    id: labelComponent
                    Label {
                        text: App.tooltip?.tooltipSections[sectionIndex]
                        horizontalAlignment: Text.AlignHCenter
                        font.family: "Fontin SmallCaps"
                        font.pixelSize: 17
                        color: "#7f7f7f"

                        bottomPadding: 0
                        topPadding: 0
                        leftPadding: 10
                        rightPadding: 10

                        textFormat: Text.RichText
                        wrapMode: Text.Wrap
                        background: Rectangle { color: "black" }
                    }
                }

                Component {
                    id: separatorComponent
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        color: "black"
                        width: separatorImage.implicitWidth
                        height: separatorImage.implicitHeight
                        Image {
                            id: separatorImage
                            anchors.fill: parent
                            source: App.tooltip?.separatorUrl
                        }
                    }
                }
            }
            Rectangle {
                color: "black"
                Layout.preferredHeight: 5
                Layout.fillWidth: true
            }
        }
    }
}
