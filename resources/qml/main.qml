import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: mainWindow
    property bool isRecording: false
    property var trayHandler: null
    property int recentTranscriptionsCount: 0
    property var recentTranscriptions: []

    visible: true
    width: 900
    height: 600
    title: qsTr("Vibeco - Voice Assistant")
    color: "#1A1A1A"  // Dark background

    Component.onCompleted: {
        // Access trayHandler from the QML context
        if (typeof trayHandler !== "undefined") {
            mainWindow.trayHandler = trayHandler;
            console.log("TrayHandler set successfully");
        } else {
            console.error("TrayHandler is not available in QML context");
        }
    }

    // Header
    Rectangle {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 60
        color: "#2D2D2D"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20

            Image {
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                source: "qrc:/icons/app-icon.png"
                fillMode: Image.PreserveAspectFit
            }

            Label {
                text: qsTr("Vibeco")
                font.pixelSize: 22
                font.family: interBold.name
                color: "white"
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Settings")
                onClicked: {
                    if (mainWindow.trayHandler) {
                        mainWindow.trayHandler.showSettings()
                    }
                }
                background: Rectangle {
                    color: parent.pressed ? "#404040" : "#333333"
                    radius: 5
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 14
                    font.family: interMedium.name
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Layout.preferredHeight: 36
                Layout.preferredWidth: 100
            }
        }
    }

    // Main content area
    ColumnLayout {
        anchors {
            top: header.bottom
            bottom: bottomBar.top
            left: parent.left
            right: parent.right
            margins: 20
        }
        spacing: 20

        // Welcome section
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            color: "#2D2D2D"
            radius: 10

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Label {
                    text: qsTr("Welcome to Vibeco")
                    font.pixelSize: 24
                    font.family: interBold.name
                    color: "white"
                }

                Label {
                    text: qsTr("Vibeco helps you easily capture your voice and convert it to text. Click the record button or use the system tray to get started.")
                    font.pixelSize: 16
                    font.family: interRegular.name
                    color: "#CCCCCC"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Button {
                    text: isRecording ? qsTr("Stop Recording") : qsTr("Start Recording")
                    onClicked: {
                        if (!isRecording && mainWindow.trayHandler) {
                            mainWindow.trayHandler.startRecording()
                        } else if (isRecording && mainWindow.trayHandler) {
                            mainWindow.trayHandler.stopRecording()
                        }
                    }
                    background: Rectangle {
                        color: isRecording ? "#E53935" : "#1976D2"
                        radius: 5
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.family: interMedium.name
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 160
                    Layout.topMargin: 10
                }
            }
        }

        // Recent Transcriptions section
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#2D2D2D"
            radius: 10

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Recent Transcriptions")
                        font.pixelSize: 20
                        font.family: interBold.name
                        color: "white"
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Clear All")
                        visible: mainWindow.recentTranscriptionsCount > 0
                        onClicked: {
                            // Clear transcriptions logic
                            mainWindow.recentTranscriptions = []
                            mainWindow.recentTranscriptionsCount = 0
                        }
                        background: Rectangle {
                            color: parent.pressed ? "#404040" : "#333333"
                            radius: 5
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#CCCCCC"
                            font.pixelSize: 12
                            font.family: interMedium.name
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.preferredHeight: 30
                        Layout.preferredWidth: 80
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ListView {
                        id: transcriptionsList
                        anchors.fill: parent
                        model: mainWindow.recentTranscriptions
                        spacing: 10

                        delegate: Rectangle {
                            width: transcriptionsList.width
                            height: transcriptionColumn.height + 30
                            color: "#333333"
                            radius: 5

                            ColumnLayout {
                                id: transcriptionColumn
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: 15
                                }
                                spacing: 5

                                RowLayout {
                                    Layout.fillWidth: true

                                    Label {
                                        text: modelData.timestamp || "Unknown"
                                        font.pixelSize: 12
                                        font.family: interMedium.name
                                        color: "#AAAAAA"
                                    }

                                    Label {
                                        text: modelData.duration ? qsTr("%1 sec").arg(modelData.duration.toFixed(1)) : ""
                                        font.pixelSize: 12
                                        font.family: interMedium.name
                                        color: "#AAAAAA"
                                        Layout.fillWidth: true
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }

                                Label {
                                    text: modelData.text || qsTr("No text available")
                                    font.pixelSize: 14
                                    font.family: interRegular.name
                                    color: "white"
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.topMargin: 5

                                    Button {
                                        text: qsTr("Copy")
                                        onClicked: {
                                            // Copy text logic
                                        }
                                        background: Rectangle {
                                            color: parent.pressed ? "#404040" : "#333333"
                                            border.color: "#555555"
                                            border.width: 1
                                            radius: 3
                                        }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#CCCCCC"
                                            font.pixelSize: 11
                                            font.family: interMedium.name
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        Layout.preferredHeight: 24
                                        Layout.preferredWidth: 60
                                    }

                                    Button {
                                        text: qsTr("Delete")
                                        onClicked: {
                                            // Remove item logic
                                            var newList = []
                                            for (var i = 0; i < mainWindow.recentTranscriptions.length; i++) {
                                                if (i !== index) {
                                                    newList.push(mainWindow.recentTranscriptions[i])
                                                }
                                            }
                                            mainWindow.recentTranscriptions = newList
                                            mainWindow.recentTranscriptionsCount = newList.length
                                        }
                                        background: Rectangle {
                                            color: parent.pressed ? "#404040" : "#333333"
                                            border.color: "#555555"
                                            border.width: 1
                                            radius: 3
                                        }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#CCCCCC"
                                            font.pixelSize: 11
                                            font.family: interMedium.name
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        Layout.preferredHeight: 24
                                        Layout.preferredWidth: 60
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }
                                }
                            }
                        }

                        // Empty state placeholder
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width * 0.8
                            height: 100
                            color: "transparent"
                            visible: mainWindow.recentTranscriptionsCount === 0

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 10

                                Label {
                                    text: qsTr("No transcriptions yet")
                                    font.pixelSize: 18
                                    font.family: interMedium.name
                                    color: "#888888"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Label {
                                    text: qsTr("Use the record button to start transcribing your voice")
                                    font.pixelSize: 14
                                    font.family: interRegular.name
                                    color: "#666666"
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Bottom control bar
    BottomControlBar {
        id: bottomBar
    }

    // Status indicator - shows when recording is in progress
    Rectangle {
        visible: isRecording
        anchors {
            top: parent.top
            right: parent.right
            margins: 10
        }
        width: statusLayout.width + 20
        height: statusLayout.height + 10
        color: "#E53935"
        radius: 5

        RowLayout {
            id: statusLayout
            anchors.centerIn: parent
            spacing: 5

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: "white"

                SequentialAnimation on opacity {
                    running: isRecording
                    loops: Animation.Infinite
                    PropertyAnimation { to: 0.3; duration: 800 }
                    PropertyAnimation { to: 1.0; duration: 800 }
                }
            }

            Label {
                text: qsTr("Recording")
                font.pixelSize: 12
                font.family: interMedium.name
                color: "white"
            }
        }
    }

    // Connections to handle recording state changes
    Connections {
        target: mainWindow.trayHandler
        enabled: mainWindow.trayHandler !== null

        function onRecordingStarted() {
            mainWindow.isRecording = true
        }

        function onRecordingStopped() {
            mainWindow.isRecording = false
        }

        function onTranscriptionReceived(text, duration, language) {
            // Add new transcription to the list
            var newTranscription = {
                text: text,
                timestamp: new Date().toLocaleString(),
                duration: duration || 0,
                language: language || "unknown"
            }

            // Add to the beginning of the list
            var newList = [newTranscription].concat(mainWindow.recentTranscriptions)
            mainWindow.recentTranscriptions = newList
            mainWindow.recentTranscriptionsCount = newList.length
        }
    }

    // Function to check if trayHandler is available
    function checkTrayHandler() {
        console.log("Checking trayHandler availability:", mainWindow.trayHandler !== null)
        return mainWindow.trayHandler !== null
    }

    // Function to be called from C++ via invokeMethod
    function onTranscriptionReceived(transcription) {
        // Add new transcription to the list
        // Add to the beginning of the list
        var newList = [transcription].concat(mainWindow.recentTranscriptions)
        mainWindow.recentTranscriptions = newList
        mainWindow.recentTranscriptionsCount = newList.length
    }

    // Font loaders
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