import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition
import Acquisition.qml

Item {
    id: characterTab

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: characterHeader
            Layout.fillWidth: true
            clip: true
            syncView: characterView
        }

        TableView {
            id: characterView
            Layout.fillWidth: true
            Layout.fillHeight: true
            columnSpacing: 2
            rowSpacing: 2
            clip: true

            model: App.characterModel

            delegate: TableViewDelegate {
                text: modelData
                implicitWidth: 100
            }
            ScrollBar.horizontal: ScrollBar {}
            ScrollBar.vertical:   ScrollBar {}
        }
    }

}
