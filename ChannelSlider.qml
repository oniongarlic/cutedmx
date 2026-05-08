import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    spacing: 8

    property alias value: ch.value    
    property alias channelLabel: chl.text

    //Layout.minimumWidth: chl.width*3

    property bool showButtons: true

    Layout.alignment: Qt.AlignHCenter

    Label {
        id: chl
        font.bold: true
        Layout.alignment: Qt.AlignHCenter
    }

    Slider {
        Layout.fillWidth: true
        id: ch
        from: 0
        to: 255
        stepSize: 1
        snapMode: Slider.SnapAlways
        orientation: Qt.Vertical
        wheelEnabled: true
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
    }

    NumberAnimation {
        id: fadeOut
        target: ch
        property: "value"
        duration: 500
        from: ch.value
        to: 0
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: fadeIn
        target: ch
        property: "value"
        duration: 500
        from: ch.value
        to: 255
        easing.type: Easing.InOutQuad
    }

    ColumnLayout {
        id: quickButtons
        visible: showButtons
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: "In"
            onClicked: {
                fadeIn.start()
            }
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: "Out"
            onClicked: {
                fadeOut.start()
            }
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: "Zero"
            onClicked: {
                ch.value=0;
            }
        }
    }

    Label {
        text: Math.round(ch.value)
        Layout.alignment: Qt.AlignHCenter
    }

}
