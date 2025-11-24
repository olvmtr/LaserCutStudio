import QtQuick
import QtQuick.Window

Window {
    id: window
    width: 1280
    height: 720
    minimumWidth: 1024
    minimumHeight: 600
    visible: true
    title: qsTr("LaserCutStudio - Éditeur de géométrie contrainte")

    // Éditeur de géométrie contrainte
    ConstraintEditorView {
        anchors.fill: parent
    }
}
