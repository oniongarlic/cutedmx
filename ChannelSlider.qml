import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    spacing: 8

    property alias value: ch.value    
    property alias channelLabel: chl.text

    width: chl.width*3

    Label {
        id: chl
        Layout.alignment: Qt.AlignHCenter
    }

    Slider {
        id: ch
        from: 0
        to: 255
        stepSize: 1
        snapMode: Slider.SnapAlways
        orientation: Qt.Vertical
        wheelEnabled: true
        Layout.alignment: Qt.AlignHCenter
    }

    NumberAnimation {
        id: fadeOut
        target: ch
        property: "value"
        duration: 500
        from: 255
        to: 0
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: fadeIn
        target: ch
        property: "value"
        duration: 500
        from: 0
        to: 255
        easing.type: Easing.InOutQuad
    }

    ColumnLayout {
        Layout.fillWidth: false
        Button {
            text: "In"
            onClicked: {
                fadeIn.start()
            }
        }
        Button {
            text: "Out"
            onClicked: {
                fadeOut.start()
            }
        }
    }

    Label {
        text: Math.round(ch.value)
        Layout.alignment: Qt.AlignHCenter
    }

}
