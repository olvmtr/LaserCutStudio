import QtQuick
import QtQuick.Window

Window {
    id: window
    width: 1280
    height: 720
    minimumWidth: 1024
    minimumHeight: 600
    visible: true
    title: qsTr("LaserCutStudio - Éditeur 2D")

    // Charger l'interface complète de l'éditeur
    EditorView {
        anchors.fill: parent
    }
}
