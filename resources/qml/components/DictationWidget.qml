import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    id: root
    width: 120
    height: isHovered || isRecording ? 35 : 5
    radius: 10
    color: "#000000"
    opacity: 0.8

    // Public properties
    property bool isRecording: false

    // Private properties
    property bool isHovered: false

    // Signals
    signal clicked

    // Animation properties
    Behavior on height {
        NumberAnimation {
            duration: 150
            easing.type: Easing.OutCubic
        }
    }

    // Recording indicator
    Rectangle {
        id: recordIndicator
        width: 10
        height: 10
        radius: 5
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        color: isRecording ? "#FF3B30" : "transparent"
        visible: isRecording || root.height > 10
    }

    // Label
    Label {
        id: statusLabel
        anchors.centerIn: parent
        text: isRecording ? "Recording..." : "Click to record"
        color: "white"
        font.pixelSize: 12
        opacity: root.height > 15 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }

    // Mouse area for interactions
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            isHovered = true;
        }

        onExited: {
            isHovered = false;
        }

        onClicked: {
            root.clicked();
        }
    }
}
