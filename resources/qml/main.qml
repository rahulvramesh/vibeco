import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls 2.15

ApplicationWindow {
    id: mainWindow
    property bool isRecording: false
    property var trayHandler: null
    visible: true
    width: 800
    height: 600
    title: qsTr("Vibeco")

    Component.onCompleted: {
        mainWindow.trayHandler = trayHandler
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Label {
            text: qsTr("Welcome to Vibeco")
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }

        // Add more UI components here

        Button {
            text: "Test Notification"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: 20
            width: 200
            height: 50
            font.pixelSize: 16
            background: Rectangle {
                color: parent.down ? "#4CAF50" : "#8BC34A"
                radius: 5
            }
            contentItem: Text {
                text: parent.text
                font: parent.font
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                shortcutManager.testNotification()
            }
        }

        Button {
            id: startRecordingButton
            text: "Start Recording"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: 20
            width: 200
            height: 50
            font.pixelSize: 16
            enabled: mainWindow.trayHandler !== null && !isRecording
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#4CAF50" : "#8BC34A") : "#CCCCCC"
                radius: 5
            }
            contentItem: Text {
                text: parent.text
                font: parent.font
                color: parent.enabled ? "white" : "#666666"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                if (mainWindow.trayHandler) {
                    mainWindow.trayHandler.startRecording()
                }
            }
        }

        Button {
            id: stopRecordingButton
            text: "Stop Recording"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: 20
            width: 200
            height: 50
            font.pixelSize: 16
            enabled: mainWindow.trayHandler !== null && isRecording
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#4CAF50" : "#8BC34A") : "#CCCCCC"
                radius: 5
            }
            contentItem: Text {
                text: parent.text
                font: parent.font
                color: parent.enabled ? "white" : "#666666"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                if (mainWindow.trayHandler) {
                    mainWindow.trayHandler.stopRecording()
                }
            }
        }

        Label {
            text: mainWindow.trayHandler ? (isRecording ? "Recording..." : "Not Recording") : "Initializing..."
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
        }
    }

    Connections {
        target: mainWindow.trayHandler
        enabled: mainWindow.trayHandler !== null
        function onRecordingStarted() {
            mainWindow.isRecording = true
        }
        function onRecordingStopped() {
            mainWindow.isRecording = false
        }
    }

    FontLoader {
        id: interRegular
        source: "qrc:/fonts/Inter-Regular.ttf"
    }

    FontLoader {
        id: interMedium
        source: "qrc:/fonts/Inter-Medium.ttf"
    }

    FontLoader {
        id: interBold
        source: "qrc:/fonts/Inter-Bold.ttf"
    }
} 
