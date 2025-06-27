import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition
import Acquisition.qml

Item {
    id: stashTab

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal


        ColumnLayout {
            Button {
                text: "List Stashes"
                onClicked: App.getStashesTemp()
            }
            Button {
                text: "Get Stashes"
                onClicked: App.getStashesTemp2()
            }
        }

        ColumnLayout {

            HorizontalHeaderView {
                id: stashHeader
                clip: true
                syncView: stashView
                Layout.fillWidth: true
            }

            TableView {
                id: stashView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                model: App.stashModel

                delegate: TableViewDelegate {
                    text: modelData
                    implicitWidth: 100
                }

                ScrollBar.horizontal: ScrollBar {}
                ScrollBar.vertical:   ScrollBar {}
            }
        }

    }



}
