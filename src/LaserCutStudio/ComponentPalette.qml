/**
 * @file ComponentPalette.qml
 * @brief Palette simplifiée - drag & drop fonctionnel
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: palette
    color: "#2c3e50"

    // Header
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: "#1a252f"

        Label {
            anchors.centerIn: parent
            text: "🎮 Components"
            font.pixelSize: 20
            font.bold: true
            color: "#ecf0f1"
        }
    }

    // Liste scrollable
    ScrollView {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

        Column {
            width: parent.width
            spacing: 8

            // Point
            ComponentItem {
                width: parent.width
                componentType: "Point"
                icon: "●"
                bgColor: "#e74c3c"
            }

            // Segment
            ComponentItem {
                width: parent.width
                componentType: "Segment"
                icon: "━"
                bgColor: "#3498db"
            }

            // Circle
            ComponentItem {
                width: parent.width
                componentType: "Circle"
                icon: "○"
                bgColor: "#2ecc71"
            }

            // Rectangle
            ComponentItem {
                width: parent.width
                componentType: "Rectangle"
                icon: "▭"
                bgColor: "#f39c12"
            }

            // Arc
            ComponentItem {
                width: parent.width
                componentType: "Arc"
                icon: "⌒"
                bgColor: "#9b59b6"
            }
        }
    }

    // Composant draggable
    component ComponentItem: Rectangle {
        id: item
        property string componentType: ""
        property string icon: ""
        property color bgColor: "white"

        height: 80
        color: bgColor
        radius: 8
        border.color: "#1a252f"
        border.width: 2

        // État draggé
        states: State {
            when: dragHandler.active
            PropertyChanges {
                target: item
                opacity: 0.5
            }
        }

        // Contenu
        Row {
            anchors.centerIn: parent
            spacing: 15

            Label {
                text: item.icon
                font.pixelSize: 36
                font.bold: true
                color: "#2c3e50"
            }

            Label {
                text: item.componentType
                font.pixelSize: 18
                font.bold: true
                color: "#2c3e50"
            }
        }

        // Drag handler
        DragHandler {
            id: dragHandler

            onActiveChanged: {
                if (active) {
                    console.log("Drag started:", item.componentType);
                    parent.grabToImage(function(result) {
                        parent.Drag.imageSource = result.url;
                    });
                }
            }
        }

        // Drag properties
        Drag.active: dragHandler.active
        Drag.dragType: Drag.Automatic
        Drag.supportedActions: Qt.CopyAction
        Drag.mimeData: { "text/plain": componentType }
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        // Tooltip
        ToolTip.visible: hoverHandler.hovered
        ToolTip.text: "Drag " + componentType + " to canvas"
        ToolTip.delay: 500

        HoverHandler {
            id: hoverHandler
        }
    }
}
