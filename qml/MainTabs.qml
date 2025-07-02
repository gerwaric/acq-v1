import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Acquisition
//import Acquisition.qml

Item {
    anchors.fill: parent
    //enabled: App.isAuthenticated

    Settings {
        id: settings
        property alias tabIndex: tabs.currentIndex
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        enabled: App.isAuthenticated

        TabBar {
            id: tabs
            Layout.fillWidth: parent
            TabButton { padding: 10; text: "Characters" }
            TabButton { padding: 10; text: "Stash Tabs" }
            TabButton { padding: 10; text: "Items" }
        }

        StackLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: tabs.currentIndex

            CharactersTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            StashesTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            ItemsTab {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

        }
    }


}
