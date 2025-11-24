/**
 * @file ConstraintEditorView.qml
 * @brief Interface de test pour l'édition avec géométrie contrainte
 */

import QtQuick
import QtQuick.Controls
import LaserCutStudio

Rectangle {
    id: root
    color: "#f0f0f0"

    // ConstraintSketch partagé
    property var sketch: null

    Component.onCompleted: {
        // Créer un sketch au démarrage
        sketch = Qt.createQmlObject('import LaserCutStudio; ConstraintSketch { name: "Test Sketch" }', root);
        canvas.sketch = sketch;
        console.log("Constraint Editor initialized with new sketch");
    }

    // Toolbar en haut
    Rectangle {
        id: toolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "#2c3e50"

        Row {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5

            // Boutons de mode
            ToolButton {
                text: "Draw Segment"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.DrawSegment
                onClicked: canvas.editMode = ConstraintCanvas2DView.DrawSegment
                ToolTip.text: "Dessiner des segments (crée les points automatiquement)"
                ToolTip.visible: hovered
            }

            ToolButton {
                text: "Select"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.Select
                onClicked: canvas.editMode = ConstraintCanvas2DView.Select
                ToolTip.text: "Sélectionner et déplacer les éléments"
                ToolTip.visible: hovered
            }

            ToolButton {
                text: "Add Constraint"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.AddConstraint
                onClicked: canvas.editMode = ConstraintCanvas2DView.AddConstraint
                ToolTip.text: "Ajouter des contraintes entre éléments"
                ToolTip.visible: hovered
            }

            Rectangle { width: 2; height: parent.height; color: "#34495e" }

            // Boutons Undo/Redo
            ToolButton {
                text: "Undo"
                enabled: canvas.canUndo()
                onClicked: canvas.undo()
                ToolTip.text: "Annuler (Ctrl+Z)"
                ToolTip.visible: hovered
            }

            ToolButton {
                text: "Redo"
                enabled: canvas.canRedo()
                onClicked: canvas.redo()
                ToolTip.text: "Refaire (Ctrl+Y)"
                ToolTip.visible: hovered
            }

            Rectangle { width: 2; height: parent.height; color: "#34495e" }

            // Type de contrainte
            Label {
                text: "Constraint:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            ComboBox {
                id: constraintTypeCombo
                model: ["Coincident", "Parallel", "Perpendicular", "Equal Length"]
                onCurrentIndexChanged: {
                    switch(currentIndex) {
                        case 0: canvas.constraintType = ConstraintCanvas2DView.Coincident; break;
                        case 1: canvas.constraintType = ConstraintCanvas2DView.Parallel; break;
                        case 2: canvas.constraintType = ConstraintCanvas2DView.Perpendicular; break;
                        case 3: canvas.constraintType = ConstraintCanvas2DView.EqualLength; break;
                    }
                }
            }

            Rectangle { width: 2; height: parent.height; color: "#34495e" }

            // Sélecteur d'unités
            Label {
                text: "Unit:"
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            ComboBox {
                id: unitCombo
                model: ["mm", "cm", "in"]
                currentIndex: 0
                onCurrentIndexChanged: {
                    if (sketch) {
                        switch(currentIndex) {
                            case 0: sketch.setUnit(0); break; // Millimeters
                            case 1: sketch.setUnit(1); break; // Centimeters
                            case 2: sketch.setUnit(2); break; // Inches
                        }
                    }
                }
            }

            Rectangle { width: 2; height: parent.height; color: "#34495e" }

            // Actions
            ToolButton {
                text: "Solve"
                onClicked: canvas.solve()
            }

            ToolButton {
                text: "Clear"
                onClicked: canvas.clear()
            }

            ToolButton {
                text: "Reset View"
                onClicked: canvas.resetView()
            }

            Rectangle { width: 2; height: parent.height; color: "#34495e" }

            // Options d'affichage
            CheckBox {
                text: "Grid"
                checked: canvas.gridVisible
                onCheckedChanged: canvas.gridVisible = checked
                palette.text: "white"
            }

            CheckBox {
                text: "Snap"
                checked: canvas.snapToGrid
                onCheckedChanged: canvas.snapToGrid = checked
                palette.text: "white"
            }

            CheckBox {
                text: "Constraints"
                checked: canvas.showConstraints
                onCheckedChanged: canvas.showConstraints = checked
                palette.text: "white"
            }

            CheckBox {
                text: "Measurements"
                checked: canvas.showMeasurements
                onCheckedChanged: canvas.showMeasurements = checked
                palette.text: "white"
            }

            CheckBox {
                text: "Auto Solve"
                checked: canvas.autoSolve
                onCheckedChanged: canvas.autoSolve = checked
                palette.text: "white"
            }
        }
    }

    // Palette de composants à gauche (style Kerbal)
    ComponentPalette {
        id: componentPalette
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.bottom: statusBar.top
        width: 200

        onComponentDropped: (componentType, x, y) => {
            console.log("Component dropped:", componentType, "at", x, y)
            // Le canvas recevra le drop via DropArea
        }
    }

    // Canvas de dessin avec DropArea
    Item {
        id: canvasContainer
        anchors.top: toolbar.bottom
        anchors.left: componentPalette.right
        anchors.right: propertiesPanel.left
        anchors.bottom: statusBar.top

        ConstraintCanvas2DView {
            id: canvas
            anchors.fill: parent

            gridVisible: true
            snapToGrid: true
            showConstraints: true
            showMeasurements: true
            autoSolve: true
            gridSize: 10.0

            onPointCreated: (point) => {
                console.log("Point created at (" + point.x + ", " + point.y + ")");
                propertiesPanel.updateElementConstraints();
            }

            onSegmentCreated: (segment) => {
                console.log("Segment created with length: " + segment.length());
                propertiesPanel.updateElementConstraints();
            }

            onConstraintCreated: (constraint) => {
                console.log("Constraint created: " + constraint.getTypeName());
                propertiesPanel.updateElementConstraints();
            }

            onElementSelected: (element) => {
                console.log("Element selected: " + element.getTypeName());
                propertiesPanel.selectedElement = element;
            }
        }

        // DropArea pour recevoir les composants
        DropArea {
            id: dropArea
            anchors.fill: parent
            keys: ["text/plain"]

            onEntered: (drag) => {
                var componentType = drag.getDataAsString("text/plain")
                console.log("Drag entered canvas:", componentType)
                statusLabel.text = "Drop to create " + componentType
                // TODO: Highlight canvas
            }

            onExited: (drag) => {
                console.log("Drag exited canvas")
                statusLabel.text = "Ready - Drag components from left palette"
            }

            onDropped: (drop) => {
                // Récupérer le type de composant
                var componentType = drop.getDataAsString("text/plain")

                // Récupérer la position dans les coordonnées du canvas
                var canvasPos = mapToItem(canvas, drop.x, drop.y)
                console.log("Component dropped:", componentType, "at canvas pos:", canvasPos.x, canvasPos.y)

                // Créer le composant selon son type
                createComponentAtPosition(componentType, canvasPos.x, canvasPos.y)

                drop.accept(Qt.CopyAction)
                statusLabel.text = "Created " + componentType + " at (" + Math.round(canvasPos.x) + ", " + Math.round(canvasPos.y) + ")"
            }
        }
    }

    // ========== FONCTION: Créer un composant à une position ==========
    function createComponentAtPosition(componentType, x, y) {
        if (!sketch) {
            console.error("No sketch available")
            return
        }

        console.log("Creating component:", componentType, "at", x, y)

        switch(componentType) {
            case "Point":
                // Créer un point fixe
                var point = sketch.addPoint(x, y, false)  // Non verrouillé
                if (point) {
                    console.log("Point created successfully")
                    canvas.update()
                }
                break

            case "Segment":
                // Mode: Passer en DrawSegment pour que l'utilisateur dessine
                canvas.editMode = ConstraintCanvas2DView.DrawSegment
                statusLabel.text = "Click two points to draw segment"
                break

            case "Circle":
                // Créer un cercle centré à cette position
                // TODO: Demander le rayon ou utiliser une valeur par défaut
                console.log("Circle creation not yet implemented")
                statusLabel.text = "Circle creation - Coming soon!"
                break

            case "Rectangle":
                // TODO: Créer un rectangle
                console.log("Rectangle creation not yet implemented")
                statusLabel.text = "Rectangle creation - Coming soon!"
                break

            case "Arc":
                // TODO: Créer un arc
                console.log("Arc creation not yet implemented")
                statusLabel.text = "Arc creation - Coming soon!"
                break

            // Contraintes
            case "DistanceConstraint":
            case "LengthConstraint":
            case "AngleConstraint":
            case "ParallelConstraint":
            case "PerpendicularConstraint":
            case "CoincidentConstraint":
            case "EqualLengthConstraint":
                // Passer en mode AddConstraint
                canvas.editMode = ConstraintCanvas2DView.AddConstraint
                // Sélectionner le type de contrainte
                setConstraintTypeFromName(componentType)
                statusLabel.text = "Select elements to apply " + componentType
                break

            default:
                console.warn("Unknown component type:", componentType)
                statusLabel.text = "Unknown component: " + componentType
        }
    }

    // ========== FONCTION: Sélectionner type de contrainte par nom ==========
    function setConstraintTypeFromName(constraintName) {
        switch(constraintName) {
            case "CoincidentConstraint":
                canvas.constraintType = ConstraintCanvas2DView.Coincident
                constraintTypeCombo.currentIndex = 0
                break
            case "ParallelConstraint":
                canvas.constraintType = ConstraintCanvas2DView.Parallel
                constraintTypeCombo.currentIndex = 1
                break
            case "PerpendicularConstraint":
                canvas.constraintType = ConstraintCanvas2DView.Perpendicular
                constraintTypeCombo.currentIndex = 2
                break
            case "EqualLengthConstraint":
                canvas.constraintType = ConstraintCanvas2DView.EqualLength
                constraintTypeCombo.currentIndex = 3
                break
            default:
                console.warn("Unknown constraint type:", constraintName)
        }
    }

    // Panneau de propriétés à droite
    ConstraintPropertiesPanel {
        id: propertiesPanel
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: statusBar.top
        width: 300

        selectedElement: null
        sketch: root.sketch
    }

    // Barre de statut en bas
    Rectangle {
        id: statusBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        color: "#34495e"

        Label {
            id: statusLabel
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10
            text: "Ready - Drag components from left palette or use toolbar"
            color: "white"
        }
    }

    // Raccourcis clavier
    Shortcut {
        sequence: "Ctrl+Z"
        enabled: canvas.canUndo()
        onActivated: canvas.undo()
    }

    Shortcut {
        sequence: "Ctrl+Y"
        enabled: canvas.canRedo()
        onActivated: canvas.redo()
    }

}
