import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls 2.15
import "components"

ApplicationWindow {
    id: mainWindow
    property bool isRecording: false
    property var trayHandler: null
    visible: true
    width: 800
    height: 600
    title: qsTr("Vibeco")
    color: "#1A1A1A"  // Dark background

    Component.onCompleted: {
        mainWindow.trayHandler = trayHandler
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Label {
            text: qsTr("Welcome to Vibeco")
            font.pixelSize: 24
            font.family: interBold.name
            color: "white"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
        }

        // Main content area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    // Bottom control bar
    BottomControlBar {
        // The anchoring is handled inside the component
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
