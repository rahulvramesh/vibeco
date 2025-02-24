import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "components"

Window {
    id: dictationWindow
    width: 100  // Fixed width
    height: dictationWidget.height
    x: (Screen.width - width) / 2  // Center horizontally
    y: Screen.height - height - 30 // Position near bottom
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool | Qt.WindowDoesNotAcceptFocus
    color: "transparent"
    visible: false

    // Public properties and signals
    property bool isRecording: false

    signal dictationClicked()

    // Internal component
    DictationWidget {
        id: dictationWidget
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        isRecording: dictationWindow.isRecording

        onClicked: {
            dictationWindow.dictationClicked()
        }
    }

    // Public methods
    function show() {
        visible = true
    }

    function hide() {
        visible = false
    }

    function setRecording(recording) {
        isRecording = recording
    }
}