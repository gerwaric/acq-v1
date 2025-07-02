import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


import Acquisition

Item {

    ColumnLayout {

        RowLayout {

            Image {
                id: itemHeaderLeft
            }

            ColumnLayout {
                id: center

                Label {
                    text: App.itemTooltip.itemNameFirstLine
                }

                Label {
                    text: App.itemTooltip.itemNameSecondLine
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
