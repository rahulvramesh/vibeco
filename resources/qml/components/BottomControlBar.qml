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
                color: mainWindow.isRecording ? "#E53935" : "#F44336"
                radius: width / 2

                // Pulsing animation when recording
                Rectangle {
                    visible: mainWindow.isRecording
                    anchors.centerIn: parent
                    width: parent.width
                    height: parent.height
                    radius: width / 2
                    color: "white"
                    opacity: 0.3

                    SequentialAnimation on scale {
                        running: mainWindow.isRecording
                        loops: Animation.Infinite
                        PropertyAnimation { to: 1.2; duration: 1000; easing.type: Easing.OutQuad }
                        PropertyAnimation { to: 1.0; duration: 1000; easing.type: Easing.InQuad }
                    }
                }
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

            ToolTip {
                visible: recordButton.hovered
                text: mainWindow.isRecording ? qsTr("Stop Recording") : qsTr("Start Recording")
                delay: 500
                background: Rectangle {
                    color: "#333333"
                    border.color: "#555555"
                    border.width: 1
                    radius: 3
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 12
                    font.family: interRegular.name
                }
            }
        }

        // Status text
        Column {
            spacing: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            Label {
                text: mainWindow.isRecording ? qsTr("Recording...") : qsTr("Ready")
                color: "white"
                font.pixelSize: 16
                font.family: interMedium.name
            }

            Label {
                text: mainWindow.isRecording ? qsTr("Press the stop button when finished speaking") : qsTr("Press the record button or use the system tray icon to start")
                color: "#BBBBBB"
                font.pixelSize: 12
                font.family: interRegular.name
                width: parent.width
                elide: Text.ElideRight
            }
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

            onClicked: {
                mainWindow.trayHandler.showSettings()
            }

            ToolTip {
                visible: parent.hovered
                text: qsTr("Settings")
                delay: 500
                background: Rectangle {
                    color: "#333333"
                    border.color: "#555555"
                    border.width: 1
                    radius: 3
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 12
                    font.family: interRegular.name
                }
            }
        }

        // Help button
        Button {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter

            background: Rectangle {
                color: parent.pressed ? "#404040" : "#333333"
                radius: width / 2
            }

            contentItem: Text {
                text: "?"
                color: "white"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                // Show help dialog
            }

            ToolTip {
                visible: parent.hovered
                text: qsTr("Help")
                delay: 500
                background: Rectangle {
                    color: "#333333"
                    border.color: "#555555"
                    border.width: 1
                    radius: 3
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 12
                    font.family: interRegular.name
                }
            }
        }
    }
}