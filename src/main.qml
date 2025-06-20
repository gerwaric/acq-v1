// Copyright (C) 2025 Tom Holz.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts

import Acquisition

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: "Acquisition " + App.version

    menuBar: MenuBar {}

    footer: Frame {
        width: parent.width
        spacing: 10
        Label {
            text: App.rateLimitStatus
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        MainControlPanel {}

        Frame {
            Layout.fillHeight: parent
            Layout.fillWidth: parent
            TableView {
                anchors.fill: parent
                clip: true
                columnSpacing: 1
                rowSpacing: 1
                model: App.itemsModel

                delegate: Row {
                     spacing: 1
                     Rectangle {
                         width: 100; height: 30
                         color: "white"; border.width: 1
                         Text {
                             anchors.centerIn: parent
                             text: modelData.armour
                         }
                     }
                     Rectangle {
                         width: 100; height: 30
                         color: "white"; border.width: 1
                         Text {
                             anchors.centerIn: parent
                             text: model.evasion
                         }
                     }
                     Rectangle {
                         width: 100; height: 30
                         color: "white"; border.width: 1
                         Text {
                             anchors.centerIn: parent
                             text: model.energyShield
                         }
                     }
                }
            }
        }
    }

}
