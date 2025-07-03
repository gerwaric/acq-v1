import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Item {
    anchors.fill: parent

    ColumnLayout {
        spacing: 0

        RowLayout {
            Layout.fillWidth: true

            Image {
                id: itemHeaderLeft
            }

            ColumnLayout {
                id: center

                Label {
                    id: itemNameFirstLine

                }

                Label {
                    id: itemNameSecondLine
                }
            }

            Image {
                id: itemHeaderRight
            }

        }

        Label {
            id: properties
        }

    }

}
