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

    ColumnLayout {
        Layout.fillWidth: false
        Button {
            text: "FU"
            onClicked: {

            }
        }
        Button {
            text: "FD"
            onClicked: {

            }
        }
    }

    Label {
        text: ch.value
        Layout.alignment: Qt.AlignHCenter
    }

}
