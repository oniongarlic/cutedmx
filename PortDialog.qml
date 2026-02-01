import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import org.tal.dmx

Dialog {
    id: nyaDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    //width: Math.min(parent.width/2, 600)
    //height: Math.min(parent.height/1.5, 400)
    title: "Choose device"
    modal: true

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    property alias device: deviceCombo.currentText

    ColumnLayout {
        anchors.fill: parent
        Label {
            text: "DMX Serial port"
        }
        ComboBox {
            id: deviceCombo
            model: SerialPortInfoModel {}

                textRole: "portName"

                delegate: ItemDelegate {
                    text: portName + " (" + description + " / "+ serialNumber + ")"
                }
        }
    }
}
