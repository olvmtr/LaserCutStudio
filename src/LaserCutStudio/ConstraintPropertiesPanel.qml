/**
 * @file ConstraintPropertiesPanel.qml
 * @brief Panneau de propriétés pour gérer les contraintes
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LaserCutStudio

Rectangle {
    id: root
    color: "#ecf0f1"
    border.color: "#bdc3c7"
    border.width: 1

    property var selectedElement: null
    property var sketch: null

    // Liste des contraintes affectant l'élément sélectionné
    property var elementConstraints: []

    onSelectedElementChanged: updateElementConstraints()

    function updateElementConstraints() {
        elementConstraints = [];
        if (!selectedElement || !sketch) return;

        var allConstraints = sketch.constraints();
        for (var i = 0; i < allConstraints.length; i++) {
            var constraint = allConstraints[i];
            var affectedPoints = constraint.affectedPoints();

            // Vérifier si l'élément sélectionné est affecté par cette contrainte
            for (var j = 0; j < affectedPoints.length; j++) {
                if (affectedPoints[j] === selectedElement) {
                    elementConstraints.push(constraint);
                    break;
                }
            }
        }

        constraintListModel.clear();
        for (var k = 0; k < elementConstraints.length; k++) {
            constraintListModel.append({
                "constraint": elementConstraints[k],
                "typeName": elementConstraints[k].getTypeName(),
                "locked": elementConstraints[k].isLocked(),
                "error": elementConstraints[k].error().toFixed(3),
                "satisfied": elementConstraints[k].isSatisfied(0.001)
            });
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // En-tête
        Label {
            text: "Constraint Properties"
            font.bold: true
            font.pixelSize: 16
            Layout.fillWidth: true
        }

        Rectangle {
            height: 1
            Layout.fillWidth: true
            color: "#bdc3c7"
        }

        // Informations élément sélectionné
        GroupBox {
            title: "Selected Element"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Label {
                    text: selectedElement ?
                          "Type: " + selectedElement.getTypeName() :
                          "No element selected"
                }

                Label {
                    visible: selectedElement !== null
                    text: selectedElement && selectedElement.getTypeName() === "GeometricPoint" ?
                          "Position: (" + selectedElement.x.toFixed(2) + ", " + selectedElement.y.toFixed(2) + ")" :
                          ""
                }

                Label {
                    visible: selectedElement && selectedElement.getTypeName() === "GeometricSegment"
                    text: selectedElement && selectedElement.getTypeName() === "GeometricSegment" ?
                          "Length: " + selectedElement.length().toFixed(2) + " mm" :
                          ""
                }

                CheckBox {
                    visible: selectedElement && selectedElement.getTypeName() === "GeometricPoint"
                    text: "Locked"
                    checked: selectedElement ? selectedElement.isLocked() : false
                    onCheckedChanged: {
                        if (selectedElement && selectedElement.getTypeName() === "GeometricPoint") {
                            selectedElement.setLocked(checked);
                        }
                    }
                }
            }
        }

        // Transformations
        GroupBox {
            title: "Transformations"
            Layout.fillWidth: true
            visible: selectedElement !== null

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                // Transformations pour points
                ColumnLayout {
                    visible: selectedElement && selectedElement.getTypeName() === "GeometricPoint"
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        text: "Move Point:"
                        font.bold: true
                        font.pixelSize: 11
                    }

                    GridLayout {
                        columns: 3
                        Layout.fillWidth: true

                        Label {
                            text: "X:"
                            font.pixelSize: 10
                        }
                        SpinBox {
                            id: pointXSpinBox
                            from: -10000
                            to: 10000
                            value: selectedElement ? selectedElement.x : 0
                            onValueModified: {
                                if (selectedElement && selectedElement.getTypeName() === "GeometricPoint") {
                                    selectedElement.setX(value);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                            editable: true
                            Layout.fillWidth: true
                        }
                        Label { text: "mm" }

                        Label {
                            text: "Y:"
                            font.pixelSize: 10
                        }
                        SpinBox {
                            id: pointYSpinBox
                            from: -10000
                            to: 10000
                            value: selectedElement ? selectedElement.y : 0
                            onValueModified: {
                                if (selectedElement && selectedElement.getTypeName() === "GeometricPoint") {
                                    selectedElement.setY(value);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                            editable: true
                            Layout.fillWidth: true
                        }
                        Label { text: "mm" }
                    }

                    Row {
                        spacing: 5
                        Layout.fillWidth: true

                        Button {
                            text: "↑ 10"
                            onClicked: {
                                if (selectedElement) {
                                    selectedElement.setY(selectedElement.y - 10);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                    pointYSpinBox.value = selectedElement.y;
                                }
                            }
                        }
                        Button {
                            text: "↓ 10"
                            onClicked: {
                                if (selectedElement) {
                                    selectedElement.setY(selectedElement.y + 10);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                    pointYSpinBox.value = selectedElement.y;
                                }
                            }
                        }
                        Button {
                            text: "← 10"
                            onClicked: {
                                if (selectedElement) {
                                    selectedElement.setX(selectedElement.x - 10);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                    pointXSpinBox.value = selectedElement.x;
                                }
                            }
                        }
                        Button {
                            text: "→ 10"
                            onClicked: {
                                if (selectedElement) {
                                    selectedElement.setX(selectedElement.x + 10);
                                    if (sketch && sketch.autoSolve) sketch.solve();
                                    pointXSpinBox.value = selectedElement.x;
                                }
                            }
                        }
                    }
                }

                // Transformations pour segments
                ColumnLayout {
                    visible: selectedElement && selectedElement.getTypeName() === "GeometricSegment"
                    Layout.fillWidth: true
                    spacing: 5

                    Label {
                        text: "Transform Segment:"
                        font.bold: true
                        font.pixelSize: 11
                    }

                    Row {
                        spacing: 5
                        Layout.fillWidth: true

                        Button {
                            text: "↻ 45°"
                            ToolTip.text: "Rotate segment 45° CW around midpoint"
                            ToolTip.visible: hovered
                            onClicked: {
                                if (selectedElement) {
                                    var mid = selectedElement.midpoint();
                                    var start = selectedElement.start();
                                    var end = selectedElement.end();

                                    // Rotation de 45° autour du milieu
                                    var angle = 45 * Math.PI / 180;
                                    var cos = Math.cos(angle);
                                    var sin = Math.sin(angle);

                                    // Rotate start
                                    var dx1 = start.x - mid.x;
                                    var dy1 = start.y - mid.y;
                                    start.setX(mid.x + dx1 * cos - dy1 * sin);
                                    start.setY(mid.y + dx1 * sin + dy1 * cos);

                                    // Rotate end
                                    var dx2 = end.x - mid.x;
                                    var dy2 = end.y - mid.y;
                                    end.setX(mid.x + dx2 * cos - dy2 * sin);
                                    end.setY(mid.y + dx2 * sin + dy2 * cos);

                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                        }

                        Button {
                            text: "↺ 45°"
                            ToolTip.text: "Rotate segment 45° CCW around midpoint"
                            ToolTip.visible: hovered
                            onClicked: {
                                if (selectedElement) {
                                    var mid = selectedElement.midpoint();
                                    var start = selectedElement.start();
                                    var end = selectedElement.end();

                                    // Rotation de -45° autour du milieu
                                    var angle = -45 * Math.PI / 180;
                                    var cos = Math.cos(angle);
                                    var sin = Math.sin(angle);

                                    // Rotate start
                                    var dx1 = start.x - mid.x;
                                    var dy1 = start.y - mid.y;
                                    start.setX(mid.x + dx1 * cos - dy1 * sin);
                                    start.setY(mid.y + dx1 * sin + dy1 * cos);

                                    // Rotate end
                                    var dx2 = end.x - mid.x;
                                    var dy2 = end.y - mid.y;
                                    end.setX(mid.x + dx2 * cos - dy2 * sin);
                                    end.setY(mid.y + dx2 * sin + dy2 * cos);

                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                        }

                        Button {
                            text: "⟷ Flip"
                            ToolTip.text: "Swap segment endpoints"
                            ToolTip.visible: hovered
                            onClicked: {
                                if (selectedElement) {
                                    var start = selectedElement.start();
                                    var end = selectedElement.end();

                                    var tempX = start.x;
                                    var tempY = start.y;
                                    start.setX(end.x);
                                    start.setY(end.y);
                                    end.setX(tempX);
                                    end.setY(tempY);

                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        Layout.fillWidth: true

                        Button {
                            text: "× 2"
                            ToolTip.text: "Scale segment length ×2"
                            ToolTip.visible: hovered
                            onClicked: {
                                if (selectedElement) {
                                    var mid = selectedElement.midpoint();
                                    var start = selectedElement.start();
                                    var end = selectedElement.end();

                                    // Scale from midpoint
                                    var dx1 = (start.x - mid.x) * 2;
                                    var dy1 = (start.y - mid.y) * 2;
                                    start.setX(mid.x + dx1);
                                    start.setY(mid.y + dy1);

                                    var dx2 = (end.x - mid.x) * 2;
                                    var dy2 = (end.y - mid.y) * 2;
                                    end.setX(mid.x + dx2);
                                    end.setY(mid.y + dy2);

                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                        }

                        Button {
                            text: "÷ 2"
                            ToolTip.text: "Scale segment length ÷2"
                            ToolTip.visible: hovered
                            onClicked: {
                                if (selectedElement) {
                                    var mid = selectedElement.midpoint();
                                    var start = selectedElement.start();
                                    var end = selectedElement.end();

                                    // Scale from midpoint
                                    var dx1 = (start.x - mid.x) * 0.5;
                                    var dy1 = (start.y - mid.y) * 0.5;
                                    start.setX(mid.x + dx1);
                                    start.setY(mid.y + dy1);

                                    var dx2 = (end.x - mid.x) * 0.5;
                                    var dy2 = (end.y - mid.y) * 0.5;
                                    end.setX(mid.x + dx2);
                                    end.setY(mid.y + dy2);

                                    if (sketch && sketch.autoSolve) sketch.solve();
                                }
                            }
                        }
                    }
                }
            }
        }

        // Liste des contraintes
        GroupBox {
            title: "Active Constraints (" + elementConstraints.length + ")"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                ListView {
                    id: constraintListView
                    model: ListModel {
                        id: constraintListModel
                    }

                    delegate: Rectangle {
                        width: constraintListView.width
                        height: constraintDelegate.height + 10
                        color: index % 2 === 0 ? "#ffffff" : "#f5f5f5"
                        border.color: model.satisfied ? "#27ae60" : "#e74c3c"
                        border.width: 1

                        ColumnLayout {
                            id: constraintDelegate
                            anchors.fill: parent
                            anchors.margins: 5
                            spacing: 5

                            RowLayout {
                                Layout.fillWidth: true

                                Label {
                                    text: model.typeName
                                    font.bold: true
                                    Layout.fillWidth: true
                                }

                                Label {
                                    text: model.satisfied ? "✓" : "✗"
                                    color: model.satisfied ? "#27ae60" : "#e74c3c"
                                    font.pixelSize: 16
                                }
                            }

                            Label {
                                text: "Error: " + model.error
                                font.pixelSize: 10
                                color: "#7f8c8d"
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 5

                                CheckBox {
                                    text: "Locked"
                                    checked: model.locked
                                    onCheckedChanged: {
                                        if (model.constraint) {
                                            model.constraint.setLocked(checked);
                                            model.locked = checked;
                                        }
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Button {
                                    text: "Delete"
                                    onClicked: {
                                        if (sketch && model.constraint) {
                                            sketch.removeConstraint(model.constraint);
                                            updateElementConstraints();
                                        }
                                    }
                                }
                            }

                            // Contrôles spécifiques selon type de contrainte
                            Loader {
                                Layout.fillWidth: true
                                sourceComponent: getConstraintControls(model.typeName, model.constraint)
                            }
                        }
                    }
                }
            }
        }

        // Boutons d'action
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Button {
                text: "Refresh"
                Layout.fillWidth: true
                onClicked: updateElementConstraints()
            }

            Button {
                text: "Solve"
                Layout.fillWidth: true
                onClicked: {
                    if (sketch) {
                        sketch.solve();
                        updateElementConstraints();
                    }
                }
            }
        }
    }

    // Fonction pour obtenir les contrôles spécifiques à chaque type de contrainte
    function getConstraintControls(typeName, constraint) {
        switch(typeName) {
            case "DistanceConstraint":
                return distanceControlsComponent;
            case "LengthConstraint":
                return lengthControlsComponent;
            case "AngleConstraint":
                return angleControlsComponent;
            default:
                return null;
        }
    }

    // Composants de contrôle pour chaque type de contrainte

    Component {
        id: distanceControlsComponent

        ColumnLayout {
            property var constraint: null

            Label {
                text: "Target Distance:"
                font.pixelSize: 10
            }

            RowLayout {
                Slider {
                    id: distanceSlider
                    from: 0
                    to: 200
                    value: constraint ? constraint.distance() : 0
                    onValueChanged: {
                        if (constraint && !distanceSpinBox.activeFocus) {
                            constraint.setDistance(value);
                        }
                    }
                    Layout.fillWidth: true
                }

                SpinBox {
                    id: distanceSpinBox
                    from: 0
                    to: 1000
                    value: constraint ? constraint.distance() : 0
                    onValueChanged: {
                        if (constraint) {
                            constraint.setDistance(value);
                            distanceSlider.value = value;
                        }
                    }
                    editable: true
                }

                Label {
                    text: "mm"
                }
            }
        }
    }

    Component {
        id: lengthControlsComponent

        ColumnLayout {
            property var constraint: null

            Label {
                text: "Target Length:"
                font.pixelSize: 10
            }

            RowLayout {
                Slider {
                    id: lengthSlider
                    from: 0
                    to: 200
                    value: constraint ? constraint.length() : 0
                    onValueChanged: {
                        if (constraint && !lengthSpinBox.activeFocus) {
                            constraint.setLength(value);
                        }
                    }
                    Layout.fillWidth: true
                }

                SpinBox {
                    id: lengthSpinBox
                    from: 0
                    to: 1000
                    value: constraint ? constraint.length() : 0
                    onValueChanged: {
                        if (constraint) {
                            constraint.setLength(value);
                            lengthSlider.value = value;
                        }
                    }
                    editable: true
                }

                Label {
                    text: "mm"
                }
            }
        }
    }

    Component {
        id: angleControlsComponent

        ColumnLayout {
            property var constraint: null

            Label {
                text: "Target Angle:"
                font.pixelSize: 10
            }

            RowLayout {
                Slider {
                    id: angleSlider
                    from: 0
                    to: 360
                    value: constraint ? constraint.angle() : 0
                    onValueChanged: {
                        if (constraint && !angleSpinBox.activeFocus) {
                            constraint.setAngle(value);
                        }
                    }
                    Layout.fillWidth: true
                }

                SpinBox {
                    id: angleSpinBox
                    from: 0
                    to: 360
                    value: constraint ? constraint.angle() : 0
                    onValueChanged: {
                        if (constraint) {
                            constraint.setAngle(value);
                            angleSlider.value = value;
                        }
                    }
                    editable: true
                }

                Label {
                    text: "°"
                }
            }
        }
    }
}
