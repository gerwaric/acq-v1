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

        ColumnLayout {
            id: resultsPanel
            SplitView.fillWidth: true

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
