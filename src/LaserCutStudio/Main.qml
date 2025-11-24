import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    width: 1280
    height: 720
    minimumWidth: 1024
    minimumHeight: 600
    visible: true
    title: qsTr("LaserCutStudio - Éditeur 2D")

    // Barre d'onglets en haut
    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        TabButton {
            text: "Formes primitives"
            width: implicitWidth
        }

        TabButton {
            text: "Géométrie contrainte"
            width: implicitWidth
        }
    }

    // Conteneur pour les différentes vues
    StackLayout {
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: tabBar.currentIndex

        // Vue 1: Éditeur de formes primitives
        EditorView {
            id: editorView
        }

        // Vue 2: Éditeur de géométrie contrainte
        ConstraintEditorView {
            id: constraintEditorView
        }
    }
}
