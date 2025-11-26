/**
 * @file ComponentPalette.qml
 * @brief Palette de composants draggable style Kerbal
 */

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    color: "#34495e"

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text {
            text: "Components"
            color: "white"
            font.pixelSize: 16
            font.bold: true
            width: parent.width
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#7f8c8d"
        }

        // Point component
        PaletteItem {
            text: "Point"
            icon: "⬤"
            componentType: "point"
        }

        // Segment component
        PaletteItem {
            text: "Segment"
            icon: "━"
            componentType: "segment"
        }

        // Rectangle component
        PaletteItem {
            text: "Rectangle"
            icon: "▭"
            componentType: "rectangle"
        }
    }

    // Composant draggable
    component PaletteItem: Rectangle {
        property string text: ""
        property string icon: ""
        property string componentType: ""

        width: parent.width
        height: 40
        color: dragArea.pressed ? "#2980b9" : (dragArea.containsMouse ? "#3498db" : "#2c3e50")
        radius: 4

        Row {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 10

            Text {
                text: icon
                color: "white"
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: parent.parent.text
                color: "white"
                font.pixelSize: 14
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id: dragArea
            anchors.fill: parent
            hoverEnabled: true

            drag.target: draggable

            onPressed: {
                draggable.visible = true;
                draggable.x = mouseX;
                draggable.y = mouseY;
            }

            onReleased: {
                draggable.visible = false;
            }
        }

        Rectangle {
            id: draggable
            width: 60
            height: 60
            color: "#3498db"
            radius: 4
            visible: false
            opacity: 0.7

            Text {
                anchors.centerIn: parent
                text: icon
                color: "white"
                font.pixelSize: 30
            }

            Drag.active: dragArea.drag.active
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2
            Drag.mimeData: { "text/plain": componentType }
        }
    }
}
