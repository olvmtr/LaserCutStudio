/**
 * ToolPalette.qml
 * Palette d'outils DYNAMIQUE basée sur les formes enregistrées via Factory Pattern
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserCutStudio 1.0

Rectangle {
    id: root

    width: 80
    color: "#2a2a2a"
    border.color: "#3a3a3a"
    border.width: 1

    // Signal émis quand un outil est sélectionné
    signal toolSelected(string toolName)

    // Propriété pour l'outil actif
    property string activeTool: ""

    // Référence à EditorService pour obtenir les types disponibles
    property EditorService editorService: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // ===== Titre =====
        Label {
            text: "Outils"
            font.bold: true
            font.pixelSize: 12
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#3a3a3a"
        }

        // ===== Outil de sélection (fixe) =====
        ToolButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            text: "➤\nSélection"
            font.pixelSize: 20

            checkable: true
            checked: root.activeTool === "Selection"

            onClicked: {
                root.toolSelected("Selection")
                root.activeTool = "Selection"
            }

            ToolTip.text: "Outil de sélection (S)\nCliquer pour sélectionner\nGlisser pour déplacer"
            ToolTip.visible: hovered
            ToolTip.delay: 500
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#3a3a3a"
        }

        // ===== Outils de dessin DYNAMIQUES =====
        // Génère automatiquement un bouton pour chaque type de forme enregistré
        Repeater {
            model: editorService ? editorService.availableShapeTypes : []

            ToolButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 60

                property string shapeType: modelData

                text: getShapeIcon(shapeType) + "\n" + shapeType
                font.pixelSize: 20

                checkable: true
                checked: root.activeTool === shapeType

                onClicked: {
                    root.toolSelected(shapeType)
                    root.activeTool = shapeType
                }

                ToolTip.text: shapeType + " (R)\nClic-glisser pour dessiner"
                ToolTip.visible: hovered
                ToolTip.delay: 500

                // Fonction pour obtenir l'icône selon le type
                function getShapeIcon(type) {
                    switch(type) {
                        case "Rectangle": return "▭"
                        case "Circle": return "○"
                        case "Triangle": return "△"
                        case "Line": return "╱"
                        case "Polygon": return "⬡"
                        default: return "◆"
                    }
                }
            }
        }

        // Spacer pour pousser vers le haut
        Item {
            Layout.fillHeight: true
        }

        // ===== Aide en bas =====
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#3a3a3a"
        }

        Label {
            id: helpLabel
            text: "?"
            font.pixelSize: 16
            font.bold: true
            color: "#888"
            Layout.alignment: Qt.AlignHCenter

            MouseArea {
                id: helpMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onClicked: {
                    console.log("Aide: Outils disponibles -", editorService.availableShapeTypes)
                }
            }

            ToolTip.text: "Aide (F1)\nAfficher l'aide des outils"
            ToolTip.visible: helpMouseArea.containsMouse
        }
    }

    // Log des types disponibles au chargement
    Component.onCompleted: {
        if (editorService) {
            console.log("ToolPalette: Formes disponibles:", editorService.availableShapeTypes)
            // Ne pas auto-activer d'outil - l'état est géré par EditorService
        }
    }
}
