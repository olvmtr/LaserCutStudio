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
                text: "Place Point"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.PlacePoint
                onClicked: canvas.editMode = ConstraintCanvas2DView.PlacePoint
            }

            ToolButton {
                text: "Draw Segment"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.DrawSegment
                onClicked: canvas.editMode = ConstraintCanvas2DView.DrawSegment
            }

            ToolButton {
                text: "Select"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.Select
                onClicked: canvas.editMode = ConstraintCanvas2DView.Select
            }

            ToolButton {
                text: "Add Constraint"
                checkable: true
                checked: canvas.editMode === ConstraintCanvas2DView.AddConstraint
                onClicked: canvas.editMode = ConstraintCanvas2DView.AddConstraint
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

    // Canvas de dessin
    ConstraintCanvas2DView {
        id: canvas
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: propertiesPanel.left
        anchors.bottom: statusBar.top
        anchors.margins: 0

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
            text: "Ready - Place points to start"
            color: "white"
        }
    }

}
