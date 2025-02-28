import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Window {
    id: dictationWindow
    width: 120  // Fixed width
    height: dictationWidget.height
    x: (Screen.width - width) / 2  // Center horizontally
    y: Screen.height - height - 30 // Position near bottom
    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
    color: "transparent"
    visible: false

    // Public properties and signals
    property bool isRecording: false

    signal dictationClicked

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
            dictationWindow.dictationClicked();
        }
    }

    // Ensure window stays on top when recording starts
    onIsRecordingChanged: {
        if (isRecording) {
            // Force window to top by clearing and resetting flags
            flags = 0;
            // macOS specifically needs these flags to properly stay on top
            flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool | Qt.BypassWindowManagerHint | Qt.WindowDoesNotAcceptFocus;
            raise();
        }
    }

    // Special handling for macOS focus events
    onActiveChanged: {
        if (!active && isRecording) {
            // When window loses focus but is recording, force it back on top
            raise();
        }
    }

    // Public methods
    function show() {
        visible = true;
        // Reset flags and raise to ensure visibility on macOS
        flags = 0;
        flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool | Qt.BypassWindowManagerHint | Qt.WindowDoesNotAcceptFocus;
        raise();
    }

    function hide() {
        visible = false;
    }

    function setRecording(recording) {
        isRecording = recording;
        if (recording) {
            visible = true;
            // Reset flags completely to force macOS to reconsider window ordering
            flags = 0;
            flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool | Qt.BypassWindowManagerHint | Qt.WindowDoesNotAcceptFocus;
            raise();
        }
    }
}
