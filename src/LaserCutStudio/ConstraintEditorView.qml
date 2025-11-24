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
        anchors.right: infoPanel.left
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
            updateInfo();
        }

        onSegmentCreated: (segment) => {
            console.log("Segment created with length: " + segment.length());
            updateInfo();
        }

        onConstraintCreated: (constraint) => {
            console.log("Constraint created: " + constraint.getTypeName());
            updateInfo();
        }

        onElementSelected: (element) => {
            console.log("Element selected: " + element.getTypeName());
        }
    }

    // Panneau d'informations à droite
    Rectangle {
        id: infoPanel
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: statusBar.top
        width: 250
        color: "#ecf0f1"
        border.color: "#bdc3c7"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Label {
                text: "Sketch Info"
                font.bold: true
                font.pixelSize: 16
            }

            Label {
                id: pointCountLabel
                text: "Points: 0"
            }

            Label {
                id: segmentCountLabel
                text: "Segments: 0"
            }

            Label {
                id: constraintCountLabel
                text: "Constraints: 0"
            }

            Rectangle { height: 1; width: parent.width; color: "#bdc3c7" }

            Label {
                text: "Zoom: " + (canvas.zoomLevel * 100).toFixed(0) + "%"
            }

            Label {
                text: "Grid Size: " + canvas.gridSize.toFixed(1) + " mm"
            }

            Rectangle { height: 1; width: parent.width; color: "#bdc3c7" }

            Label {
                text: "Mode: " + getModeText()
            }

            Label {
                text: "Constraint Type: " + constraintTypeCombo.currentText
            }
        }
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

    // Fonctions helpers
    function updateInfo() {
        if (!sketch) return;

        var elements = sketch.elements();
        var constraints = sketch.constraints();

        var pointCount = 0;
        var segmentCount = 0;

        for (var i = 0; i < elements.length; i++) {
            var typeName = elements[i].getTypeName();
            if (typeName === "GeometricPoint") pointCount++;
            else if (typeName === "GeometricSegment") segmentCount++;
        }

        pointCountLabel.text = "Points: " + pointCount;
        segmentCountLabel.text = "Segments: " + segmentCount;
        constraintCountLabel.text = "Constraints: " + constraints.length;

        statusLabel.text = "Elements: " + pointCount + " pts, " + segmentCount + " segs, " + constraints.length + " constraints";
    }

    function getModeText() {
        switch(canvas.editMode) {
            case ConstraintCanvas2DView.PlacePoint: return "Place Point";
            case ConstraintCanvas2DView.DrawSegment: return "Draw Segment";
            case ConstraintCanvas2DView.Select: return "Select";
            case ConstraintCanvas2DView.AddConstraint: return "Add Constraint";
            default: return "Unknown";
        }
    }
}
