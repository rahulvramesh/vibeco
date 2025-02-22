pragma Singleton
import QtQuick

QtObject {
    readonly property FontLoader interRegular: FontLoader {
        source: "qrc:/fonts/Inter-Regular.ttf"
    }
    
    readonly property FontLoader interMedium: FontLoader {
        source: "qrc:/fonts/Inter-Medium.ttf"
    }
    
    readonly property FontLoader interBold: FontLoader {
        source: "qrc:/fonts/Inter-Bold.ttf"
    }

    readonly property string regularFont: interRegular.name
    readonly property string mediumFont: interMedium.name
    readonly property string boldFont: interBold.name
} 