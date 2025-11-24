/**
 * @file ConstraintEditorView.qml
 * @brief Interface simplifiée style Kerbal Space Program - drag & drop uniquement
 */

import QtQuick
import QtQuick.Controls
import LaserCutStudio

Rectangle {
    id: root
    color: "#2c3e50"

    // ConstraintSketch partagé
    property var sketch: null

    Component.onCompleted: {
        sketch = Qt.createQmlObject('import LaserCutStudio; ConstraintSketch { name: "Test Sketch" }', root);
        canvas.sketch = sketch;
        console.log("Constraint Editor initialized - Kerbal mode");
    }

    // ========== PALETTE DE COMPOSANTS (GAUCHE) ==========
    ComponentPalette {
        id: componentPalette
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: statusBar.top
        width: 250
    }

    // ========== CANVAS CENTRAL ==========
    Rectangle {
        id: canvasContainer
        anchors.top: parent.top
        anchors.left: componentPalette.right
        anchors.right: propertiesPanel.left
        anchors.bottom: statusBar.top
        color: "#ecf0f1"

        ConstraintCanvas2DView {
            id: canvas
            anchors.fill: parent
            anchors.margins: 10

            gridVisible: true
            snapToGrid: true
            showConstraints: true
            showMeasurements: true
            autoSolve: true
            gridSize: 10.0

            // Mode par défaut: Select
            editMode: ConstraintCanvas2DView.Select

            onPointCreated: (point) => {
                console.log("Point created at (" + point.x + ", " + point.y + ")");
                propertiesPanel.updateElementConstraints();
            }

            onSegmentCreated: (segment) => {
                console.log("Segment created");
                propertiesPanel.updateElementConstraints();
            }

            onConstraintCreated: (constraint) => {
                console.log("Constraint created");
                propertiesPanel.updateElementConstraints();
            }

            onElementSelected: (element) => {
                console.log("Element selected");
                propertiesPanel.selectedElement = element;
            }
        }

        // DropArea pour recevoir les drops
        DropArea {
            id: dropArea
            anchors.fill: parent

            onEntered: (drag) => {
                statusLabel.text = "Release to create component";
                canvasContainer.color = "#d5dbdb";
            }

            onExited: (drag) => {
                statusLabel.text = "Drag components from left panel";
                canvasContainer.color = "#ecf0f1";
            }

            onDropped: (drop) => {
                var componentType = drop.getDataAsString("text/plain");
                var canvasPos = mapToItem(canvas, drop.x, drop.y);

                console.log("DROP:", componentType, "at", canvasPos.x, canvasPos.y);

                createComponent(componentType, canvasPos.x, canvasPos.y);

                drop.accept(Qt.CopyAction);
                canvasContainer.color = "#ecf0f1";
            }
        }

        // Mini toolbar flottante
        Rectangle {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 20
            width: 200
            height: 120
            color: "#34495e"
            opacity: 0.9
            radius: 8

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Label {
                    text: "Quick Actions"
                    color: "white"
                    font.bold: true
                }

                Button {
                    text: "Solve Constraints"
                    width: parent.width
                    onClicked: canvas.solve()
                }

                Button {
                    text: "Clear All"
                    width: parent.width
                    onClicked: canvas.clear()
                }

                Button {
                    text: "Reset View"
                    width: parent.width
                    onClicked: canvas.resetView()
                }
            }
        }
    }

    // ========== PANNEAU PROPRIÉTÉS (DROITE) ==========
    ConstraintPropertiesPanel {
        id: propertiesPanel
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: statusBar.top
        width: 300

        selectedElement: null
        sketch: root.sketch
    }

    // ========== BARRE DE STATUT (BAS) ==========
    Rectangle {
        id: statusBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#1a252f"

        Label {
            id: statusLabel
            anchors.centerIn: parent
            text: "🎮 Drag components from left panel to canvas"
            font.pixelSize: 14
            color: "#ecf0f1"
        }
    }

    // ========== FONCTION: CRÉER COMPOSANT ==========
    function createComponent(type, x, y) {
        if (!sketch) {
            console.error("No sketch!");
            return;
        }

        console.log("Creating:", type);
        statusLabel.text = "Creating " + type + "...";

        switch(type) {
            case "Point":
                var pt = sketch.addPoint(x, y, false);
                if (pt) {
                    statusLabel.text = "✓ Point created at (" + Math.round(x) + ", " + Math.round(y) + ")";
                    canvas.update();
                }
                break;

            case "Segment":
                canvas.editMode = ConstraintCanvas2DView.DrawSegment;
                statusLabel.text = "→ Click two points to draw segment";
                break;

            case "Circle":
            case "Rectangle":
            case "Arc":
                statusLabel.text = "⚠ " + type + " - Coming soon!";
                break;

            default:
                statusLabel.text = "? Unknown: " + type;
        }
    }
}
