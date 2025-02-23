import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: bottomBar
    width: parent.width
    height: 80
    color: "#2D2D2D"
    radius: 15
    anchors {
        bottom: parent.bottom
        bottomMargin: 20
        left: parent.left
        leftMargin: 20
        right: parent.right
        rightMargin: 20
    }

    RowLayout {
        anchors {
            fill: parent
            margins: 15
        }
        spacing: 15

        // Record button
        Button {
            id: recordButton
            Layout.preferredWidth: 50
            Layout.preferredHeight: 50
            Layout.alignment: Qt.AlignVCenter

            background: Rectangle {
                color: recordButton.pressed ? "#E53935" : "#F44336"
                radius: width / 2
            }

            contentItem: Text {
                text: "⏺"
                color: "white"
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                if (!mainWindow.isRecording) {
                    mainWindow.trayHandler.startRecording()
                } else {
                    mainWindow.trayHandler.stopRecording()
                }
            }
        }

        // Status text
        Label {
            text: mainWindow.isRecording ? "Recording..." : "Ready"
            color: "white"
            font.pixelSize: 16
            font.family: interMedium.name
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }

        // Settings button
        Button {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter

            background: Rectangle {
                color: parent.pressed ? "#404040" : "#333333"
                radius: width / 2
            }

            contentItem: Text {
                text: "⚙️"
                color: "white"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
} 