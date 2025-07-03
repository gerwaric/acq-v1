import QtQuick
import QtQuick.Controls

Item {
    id: helloWorld2
    width: 200
    height: 50

    Rectangle {
        anchors.fill: parent
        color: "lightblue"
        radius: 8

        Text {
            anchors.centerIn: parent
            text: "Hello, World Again!"
            font.pixelSize: 20
            color: "black"
        }
    }
}
