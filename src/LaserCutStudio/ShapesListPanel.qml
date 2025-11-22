/**
 * ShapesListPanel.qml
 * Panneau affichant la liste des formes dans l'éditeur
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserCutStudio 1.0

Rectangle {
    id: root

    // Propriété exposée : service éditeur
    required property EditorService editorService

    color: "#fafafa"
    border.color: "#ddd"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 5

        // En-tête
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Label {
                text: "Formes (" + editorService.shapeCount + ")"
                font.bold: true
                font.pixelSize: 12
                Layout.fillWidth: true
            }

            // Bouton pour désélectionner tout
            ToolButton {
                text: "✕"
                ToolTip.text: "Désélectionner tout"
                ToolTip.visible: hovered
                enabled: editorService.selection && editorService.selection.count > 0
                onClicked: {
                    if (editorService.selection) {
                        editorService.selection.clear()
                    }
                }
            }

            // Bouton pour supprimer les formes sélectionnées
            ToolButton {
                text: "🗑"
                ToolTip.text: "Supprimer la sélection"
                ToolTip.visible: hovered
                enabled: editorService.selection && editorService.selection.count > 0
                onClicked: {
                    editorService.deleteSelectedShapes()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#ddd"
        }

        // Liste des formes
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: shapesList
                width: parent.width
                model: editorService.shapeCount
                spacing: 2

                // Délégué pour chaque forme
                delegate: Rectangle {
                    id: shapeItem
                    width: shapesList.width
                    height: 32

                    // Récupérer la forme depuis l'EditorService
                    property var shape: {
                        var shapes = editorService.getShapes()
                        if (shapes && index >= 0 && index < shapes.length) {
                            return shapes[index]
                        }
                        return null
                    }

                    // Vérifier si cette forme est sélectionnée
                    property bool isSelected: {
                        if (!shape || !editorService.selection) return false
                        return editorService.selection.isSelected(shape)
                    }

                    color: isSelected ? "#cce5ff" : (shapeItemMouseArea.containsMouse ? "#e6e6e6" : "transparent")
                    border.color: isSelected ? "#66afe9" : "transparent"
                    border.width: 1
                    radius: 3

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: 6

                        // Icône de type de forme
                        Label {
                            text: {
                                if (!shapeItem.shape) return "?"
                                var type = shapeItem.shape.getTypeName()
                                if (type === "Rectangle") return "▭"
                                if (type === "Circle") return "●"
                                return "⬟"
                            }
                            font.pixelSize: 16
                            color: "#555"
                            Layout.preferredWidth: 20
                        }

                        // Nom de la forme (utilise le nom personnalisé ou type par défaut)
                        Label {
                            text: {
                                if (!shapeItem.shape) return "Forme " + (index + 1)

                                // Récupérer le nom directement (Q_PROPERTY exposée à QML)
                                var shapeName = shapeItem.shape.name

                                // Si pas de nom personnalisé, utiliser le type
                                if (!shapeName || shapeName === "") {
                                    var type = shapeItem.shape.getTypeName()
                                    return type + " " + (index + 1)
                                }

                                return shapeName
                            }
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        // Info complémentaire (taille, position)
                        Label {
                            text: {
                                if (!shapeItem.shape) return ""
                                var props = shapeItem.shape.toVariant()
                                if (props["width"] !== undefined && props["height"] !== undefined) {
                                    return Math.round(props["width"]) + "×" + Math.round(props["height"])
                                }
                                if (props["radius"] !== undefined) {
                                    return "r=" + Math.round(props["radius"])
                                }
                                return ""
                            }
                            font.pixelSize: 9
                            color: "#888"
                            Layout.preferredWidth: 50
                        }

                        // Bouton supprimer
                        ToolButton {
                            text: "×"
                            font.pixelSize: 16
                            Layout.preferredWidth: 24
                            Layout.preferredHeight: 24
                            ToolTip.text: "Supprimer cette forme"
                            ToolTip.visible: hovered

                            onClicked: {
                                if (shapeItem.shape) {
                                    // Sélectionner la forme puis la supprimer
                                    editorService.selection.clear()
                                    editorService.selection.addShape(shapeItem.shape)
                                    editorService.deleteSelectedShapes()
                                }
                            }
                        }
                    }

                    // Zone de clic pour sélection
                    MouseArea {
                        id: shapeItemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton

                        onClicked: function(mouse) {
                            if (!shapeItem.shape) return

                            // CORRECTION : Activer l'outil de sélection quand on clique depuis la liste
                            editorService.activateSelectionTool()

                            if (mouse.modifiers & Qt.ControlModifier) {
                                // Ctrl+clic : toggle sélection
                                if (editorService.selection) {
                                    editorService.selection.toggleShape(shapeItem.shape)
                                }
                            } else {
                                // Clic simple : sélectionner uniquement cette forme
                                if (editorService.selection) {
                                    editorService.selection.clear()
                                    editorService.selection.addShape(shapeItem.shape)
                                }
                            }
                        }

                        onDoubleClicked: {
                            // Double-clic : centrer la vue sur la forme (fonctionnalité future)
                            console.log("Double-clic sur forme:", shapeItem.shape ? shapeItem.shape.getTypeName() : "null")
                        }
                    }
                }

                // Message si liste vide
                Label {
                    anchors.centerIn: parent
                    text: "Aucune forme\n\nUtilisez les outils\npour dessiner"
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 11
                    color: "#999"
                    visible: shapesList.count === 0
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#ddd"
        }

        // Aide
        Label {
            text: "Clic : Sélectionner\nCtrl+Clic : Multi-sélection"
            font.pixelSize: 9
            color: "#666"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }

    // Connexion aux signaux de l'éditeur pour mise à jour
    Connections {
        target: editorService

        // Rafraîchir quand les formes changent
        function onShapeCountChanged() {
            shapesList.model = editorService.shapeCount
        }

        function onShapeAdded() {
            shapesList.model = editorService.shapeCount
        }

        function onShapeRemoved() {
            shapesList.model = editorService.shapeCount
        }
    }

    // Connexion aux signaux de sélection pour mise à jour visuelle
    Connections {
        target: editorService.selection

        function onCountChanged() {
            // Forcer le rafraîchissement de la liste
            shapesList.model = 0
            shapesList.model = editorService.shapeCount
        }

        function onShapeAdded() {
            shapesList.model = 0
            shapesList.model = editorService.shapeCount
        }

        function onShapeRemoved() {
            shapesList.model = 0
            shapesList.model = editorService.shapeCount
        }

        function onCleared() {
            shapesList.model = 0
            shapesList.model = editorService.shapeCount
        }
    }

    Component.onCompleted: {
        console.log("ShapesListPanel chargé avec", editorService.shapeCount, "formes")
    }
}
