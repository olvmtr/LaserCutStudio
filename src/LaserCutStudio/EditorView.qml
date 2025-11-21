/**
 * EditorView.qml
 * Interface complète de l'éditeur 2D
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserCutStudio 1.0

Item {
    id: root

    // Propriétés exposées
    property EditorService editor: editorService

    // ===== Toolbar en haut =====
    Rectangle {
        id: toolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "#f5f5f5"
        border.color: "#ddd"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 10

            // Titre
            Label {
                text: "LaserCutStudio - Éditeur 2D"
                font.bold: true
                font.pixelSize: 16
            }

            Item { Layout.fillWidth: true }

            // Boutons Undo/Redo
            Button {
                text: "Undo"
                enabled: editor.canUndo
                onClicked: editor.undo()
                ToolTip.text: "Annuler (Ctrl+Z)"
                ToolTip.visible: hovered
            }

            Button {
                text: "Redo"
                enabled: editor.canRedo
                onClicked: editor.redo()
                ToolTip.text: "Refaire (Ctrl+Y)"
                ToolTip.visible: hovered
            }

            // Séparateur
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: "#ddd"
            }

            // Contrôles de grille
            CheckBox {
                id: gridCheckbox
                text: "Grille"
                checked: true
            }

            CheckBox {
                id: snapCheckbox
                text: "Magnétisme"
                checked: false
            }

            SpinBox {
                id: gridSizeSpinBox
                from: 5
                to: 100
                value: 10
                editable: true
                ToolTip.text: "Taille de la grille (mm)"
                ToolTip.visible: hovered
            }

            // Séparateur
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: "#ddd"
            }

            // Zoom controls
            Label {
                text: "Zoom:"
            }

            Button {
                text: "-"
                onClicked: canvas.zoomLevel = Math.max(0.1, canvas.zoomLevel * 0.8)
            }

            Label {
                text: Math.round(canvas.zoomLevel * 100) + "%"
                font.family: "monospace"
            }

            Button {
                text: "+"
                onClicked: canvas.zoomLevel = Math.min(10.0, canvas.zoomLevel * 1.25)
            }

            Button {
                text: "Reset"
                onClicked: canvas.resetView()
            }
        }
    }

    // ===== Palette d'outils à gauche =====
    ToolPalette {
        id: toolPalette
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.bottom: statusBar.top

        editorService: editor

        activeTool: {
            if (!editor.activeTool) return "Rectangle"
            return editor.activeTool.name.replace("Create ", "")
        }

        onToolSelected: function(toolName) {
            console.log("Outil sélectionné:", toolName)

            // Tous les outils sont gérés via activateToolByShapeType
            if (editor.activateToolByShapeType(toolName)) {
                console.log("Outil activé:", toolName)
            } else {
                console.warn("Impossible d'activer l'outil:", toolName)
            }
        }
    }

    // ===== Canvas principal =====
    Canvas2DView {
        id: canvas
        anchors.top: toolbar.bottom
        anchors.left: toolPalette.right
        anchors.right: propertiesPanel.left
        anchors.bottom: statusBar.top

        editorService: editor
        gridVisible: gridCheckbox.checked
        snapToGrid: snapCheckbox.checked
        gridSize: gridSizeSpinBox.value

        // Curseur personnalisé selon l'outil actif
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: {
                if (editor.activeTool)
                    return Qt.CrossCursor
                return Qt.ArrowCursor
            }
        }
    }

    // ===== Panneau de propriétés à droite =====
    Rectangle {
        id: propertiesPanel
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: statusBar.top
        width: 250
        color: "#fafafa"
        border.color: "#ddd"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Label {
                text: "Propriétés"
                font.bold: true
                font.pixelSize: 14
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#ddd"
            }

            // Info sélection
            GroupBox {
                title: "Sélection"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5

                    Label {
                        text: editor.selection ?
                              editor.selection.count + " forme(s) sélectionnée(s)" :
                              "Aucune sélection"
                        font.pixelSize: 12
                    }
                }
            }

            // Info éditeur
            GroupBox {
                title: "Éditeur"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5

                    Label {
                        text: "Formes: " + editor.shapeCount
                        font.pixelSize: 12
                    }

                    Label {
                        text: "Undo stack: " + editor.undoStackSize
                        font.pixelSize: 12
                    }

                    Label {
                        text: "Outil actif: " + (editor.activeTool ? editor.activeTool.name : "Aucun")
                        font.pixelSize: 12
                    }
                }
            }

            // Aide
            GroupBox {
                title: "Raccourcis"
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent
                    clip: true

                    Column {
                        spacing: 5
                        width: parent.width

                        Label {
                            text: "Ctrl+Z : Annuler"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Ctrl+Y : Refaire"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Del : Supprimer"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Ctrl+C : Copier"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Ctrl+V : Coller"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Échap : Désélectionner"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Molette : Zoom (avec Ctrl)"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Clic milieu : Pan"
                            font.pixelSize: 11
                            width: parent.width
                        }
                        Label {
                            text: "Clic gauche : Dessiner"
                            font.pixelSize: 11
                            width: parent.width
                        }
                    }
                }
            }
        }
    }

    // ===== Status bar en bas =====
    Rectangle {
        id: statusBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        color: "#f5f5f5"
        border.color: "#ddd"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 20

            Label {
                text: "Zoom: " + Math.round(canvas.zoomLevel * 100) + "%"
                font.pixelSize: 11
            }

            Label {
                text: "Grille: " + (gridCheckbox.checked ? "ON" : "OFF") +
                      " (" + gridSizeSpinBox.value + "mm)"
                font.pixelSize: 11
            }

            Label {
                text: "Magnétisme: " + (snapCheckbox.checked ? "ON" : "OFF")
                font.pixelSize: 11
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Prêt"
                font.pixelSize: 11
                color: "#666"
            }
        }
    }

    // ===== Raccourcis clavier =====
    Shortcut {
        sequence: "Ctrl+Z"
        enabled: editor.canUndo
        onActivated: editor.undo()
    }

    Shortcut {
        sequence: "Ctrl+Y"
        enabled: editor.canRedo
        onActivated: editor.redo()
    }

    Shortcut {
        sequence: "Delete"
        enabled: editor.selection && editor.selection.count > 0
        onActivated: {
            // Utilise deleteSelectedShapes() pour avoir Undo/Redo
            editor.deleteSelectedShapes()
        }
    }

    Shortcut {
        sequence: "Ctrl+C"
        enabled: editor.selection && editor.selection.count > 0
        onActivated: {
            var count = editor.copySelectedShapes()
            console.log("Copied", count, "shape(s)")
        }
    }

    Shortcut {
        sequence: "Ctrl+V"
        enabled: editor.hasClipboardData()
        onActivated: {
            var count = editor.pasteShapes()
            console.log("Pasted", count, "shape(s)")
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (editor.selection) {
                editor.selection.clear()
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+0"
        onActivated: canvas.resetView()
    }

    Shortcut {
        sequence: "Ctrl+Plus"
        onActivated: canvas.zoomLevel = Math.min(10.0, canvas.zoomLevel * 1.25)
    }

    Shortcut {
        sequence: "Ctrl+Minus"
        onActivated: canvas.zoomLevel = Math.max(0.1, canvas.zoomLevel * 0.8)
    }

    // ===== Composant chargé =====
    Component.onCompleted: {
        console.log("EditorView loaded")
        console.log("EditorService:", editor)
        console.log("Canvas2DView:", canvas)
    }
}
