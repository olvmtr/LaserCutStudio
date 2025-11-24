/**
 * @file ComponentPalette.qml
 * @brief Palette de composants draggables style Kerbal Space Program
 *
 * Interface verticale à gauche avec catégories et composants géométriques
 * draggables vers le canvas principal.
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: palette
    color: "#2c3e50"

    // Signal émis quand un composant est droppé
    signal componentDropped(string componentType, real x, real y)

    // En-tête
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "#1a252f"

        Label {
            anchors.centerIn: parent
            text: "Components"
            font.pixelSize: 18
            font.bold: true
            color: "#ecf0f1"
        }
    }

    // ScrollView pour les catégories
    ScrollView {
        id: scrollView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 5

        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        ColumnLayout {
            width: scrollView.width - 10
            spacing: 10

            // ========== CATÉGORIE: GEOMETRIC PRIMITIVES ==========
            CategoryHeader {
                text: "Geometric Primitives"
            }

            // Point
            ComponentItem {
                componentType: "Point"
                componentName: "Point"
                componentIcon: "●"
                iconColor: "#e74c3c"
                description: "Create a geometric point"
            }

            // Segment
            ComponentItem {
                componentType: "Segment"
                componentName: "Segment"
                componentIcon: "━"
                iconColor: "#3498db"
                description: "Create a line segment between two points"
            }

            // Circle
            ComponentItem {
                componentType: "Circle"
                componentName: "Circle"
                componentIcon: "○"
                iconColor: "#2ecc71"
                description: "Create a circle with center and radius"
            }

            // Rectangle
            ComponentItem {
                componentType: "Rectangle"
                componentName: "Rectangle"
                componentIcon: "▭"
                iconColor: "#f39c12"
                description: "Create a rectangle"
            }

            // Arc
            ComponentItem {
                componentType: "Arc"
                componentName: "Arc"
                componentIcon: "⌒"
                iconColor: "#9b59b6"
                description: "Create a circular arc"
            }

            // ========== CATÉGORIE: CONSTRAINTS ==========
            CategoryHeader {
                text: "Constraints"
            }

            // Distance Constraint
            ComponentItem {
                componentType: "DistanceConstraint"
                componentName: "Distance"
                componentIcon: "↔"
                iconColor: "#1abc9c"
                description: "Fix distance between two points"
            }

            // Length Constraint
            ComponentItem {
                componentType: "LengthConstraint"
                componentName: "Length"
                componentIcon: "│─│"
                iconColor: "#16a085"
                description: "Fix segment length"
            }

            // Angle Constraint
            ComponentItem {
                componentType: "AngleConstraint"
                componentName: "Angle"
                componentIcon: "∠"
                iconColor: "#e67e22"
                description: "Fix angle between segments"
            }

            // Parallel Constraint
            ComponentItem {
                componentType: "ParallelConstraint"
                componentName: "Parallel"
                componentIcon: "║"
                iconColor: "#3498db"
                description: "Make segments parallel"
            }

            // Perpendicular Constraint
            ComponentItem {
                componentType: "PerpendicularConstraint"
                componentName: "Perpendicular"
                componentIcon: "┴"
                iconColor: "#9b59b6"
                description: "Make segments perpendicular"
            }

            // Coincident Constraint
            ComponentItem {
                componentType: "CoincidentConstraint"
                componentName: "Coincident"
                componentIcon: "⊕"
                iconColor: "#e74c3c"
                description: "Make points coincident"
            }

            // Equal Length Constraint
            ComponentItem {
                componentType: "EqualLengthConstraint"
                componentName: "Equal Length"
                componentIcon: "═"
                iconColor: "#f39c12"
                description: "Make segments equal length"
            }

            // ========== CATÉGORIE: SHAPES (DEPRECATED) ==========
            CategoryHeader {
                text: "Shapes (Legacy)"
                collapsed: true
            }

            // Triangle
            ComponentItem {
                componentType: "Triangle"
                componentName: "Triangle"
                componentIcon: "△"
                iconColor: "#95a5a6"
                description: "[DEPRECATED] Use constraint geometry"
                visible: false  // Masqué par défaut
            }

            // Spacer en fin
            Item { height: 20 }
        }
    }

    // ========== COMPOSANT: CategoryHeader ==========
    component CategoryHeader: Rectangle {
        property string text: ""
        property bool collapsed: false

        Layout.fillWidth: true
        height: 35
        color: "#34495e"
        radius: 5

        MouseArea {
            anchors.fill: parent
            onClicked: parent.collapsed = !parent.collapsed
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            Label {
                text: parent.parent.collapsed ? "▶" : "▼"
                color: "#ecf0f1"
                font.pixelSize: 12
            }

            Label {
                text: parent.parent.text
                font.pixelSize: 14
                font.bold: true
                color: "#ecf0f1"
                Layout.fillWidth: true
            }
        }
    }

    // ========== COMPOSANT: ComponentItem ==========
    component ComponentItem: Rectangle {
        id: item

        property string componentType: ""
        property string componentName: ""
        property string componentIcon: ""
        property color iconColor: "#ffffff"
        property string description: ""

        Layout.fillWidth: true
        height: 70
        color: dragArea.containsMouse ? "#34495e" : "#2c3e50"
        radius: 8
        border.color: dragArea.drag.active ? "#3498db" : "transparent"
        border.width: 2

        // Animation de hover
        Behavior on color {
            ColorAnimation { duration: 150 }
        }

        // Contenu
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 12

            // Icône du composant
            Rectangle {
                width: 50
                height: 50
                radius: 8
                color: item.iconColor
                opacity: dragArea.drag.active ? 0.7 : 1.0

                Label {
                    anchors.centerIn: parent
                    text: item.componentIcon
                    font.pixelSize: 28
                    font.bold: true
                    color: "#2c3e50"
                }

                Behavior on opacity {
                    NumberAnimation { duration: 150 }
                }
            }

            // Nom et description
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: item.componentName
                    font.pixelSize: 14
                    font.bold: true
                    color: "#ecf0f1"
                    Layout.fillWidth: true
                }

                Label {
                    text: item.description
                    font.pixelSize: 10
                    color: "#bdc3c7"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        // MouseArea pour drag & drop
        MouseArea {
            id: dragArea
            anchors.fill: parent
            hoverEnabled: true

            drag.target: item

            onPressed: {
                // Activer le drag
                item.Drag.active = true
                item.grabToImage(function(result) {
                    item.Drag.imageSource = result.url
                })
            }

            onReleased: {
                item.Drag.drop()
                item.x = 0  // Retour à la position d'origine
                item.y = 0
            }
        }

        // Drag properties
        Drag.active: dragArea.drag.active
        Drag.dragType: Drag.Automatic
        Drag.supportedActions: Qt.CopyAction
        Drag.mimeData: {
            "text/plain": componentType
        }
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        // Tooltip
        ToolTip {
            visible: dragArea.containsMouse && !dragArea.pressed
            text: item.componentName + "\n\n" + item.description + "\n\nDrag to canvas to create"
            delay: 500
        }
    }

}
