import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

import org.tal.dmx

ApplicationWindow {
    id: app
    width: 800
    height: 480
    visible: true
    title: qsTr("Cute DMX")

    property int channelStep: 16
    property int channels: 8
    property int split: 1

    Settings {
        id: settingsDdev
        category: "device"
    }

    Settings {
        id: settingsDmx
        category: "dmx"
    }

    Component.onCompleted: {
        let d=settingsDdev.value('device')
        if (d!='') {
            dmx.setPort(d);
            dmx.start()
        }
    }

    PortDialog {
        id: portDialog

        onAccepted: {
            dmx.setPort(device);
            dmx.start();

            settingsDdev.setValue('device', device)
        }
    }

    menuBar: MenuBar {
        Menu {
            title: "File"
            Action {
                text: "Connect..."
                enabled: !dmx.active
                onTriggered: {
                    portDialog.open()
                }
            }
            Action {
                text: "Save"
                onTriggered: {
                    let a=dmx.toJson()
                    console.debug(a)
                    settingsDmx.setValue('values', a)
                }
            }
            Action {
                text: "Load"
                onTriggered: {
                    let a=settingsDmx.value('values')
                    console.debug(a)
                    dmx.fromJson(a)
                }
            }

            MenuSeparator {

            }

            Action {
                text: "Quit"
                onTriggered: Qt.exit(0)
            }
        }

        Menu {
            title: "Channels"
            MenuItem {
                objectName: "8"
                checked: true
                checkable: true
                text: "8 (1 row)"
                ButtonGroup.group: channelsGroup
            }
            MenuItem {
                objectName: "16"
                checkable: true
                text: "16 (2 rows)"
                ButtonGroup.group: channelsGroup
            }
            MenuItem {
                objectName: "32"
                checkable: true
                text: "32 (2 rows)"
                ButtonGroup.group: channelsGroup
            }
        }
    }

    header: ToolBar {
        RowLayout {
            ToolButton {
                text: "Update"
                onClicked: {
                    dmx.updateFrame();
                }
            }
            ToolButton {
                text: "Blackout"
                onClicked: {
                    dmx.blackOut();
                }
            }
        }
    }

    ButtonGroup {
        id: channelsGroup
        onClicked: {
            if (button.objectName=='8') {
                app.channels=8
                app.split=2
            } else if (button.objectName=='16') {
                app.channels=16
                app.split=2
            } else if (button.objectName=='32') {
                app.channels=32
                app.split=2
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        ColumnLayout {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            spacing: 8
            Repeater {
                id: firep
                model: 6
                CheckBox {
                    text: "F:"+(index+1)
                }
            }
        }

        GridLayout {
            id: channelGrid
            columnSpacing: 8
            rowSpacing: 8
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            enabled: dmx.active
            rows: split
            columns: channels/rows
            uniformCellWidths: true
            Repeater {
                model: channels
                ChannelSlider {
                    Layout.fillWidth: true
                    channelLabel: index+1
                    onValueChanged: {
                        for (let fi=0;fi<firep.count;fi++) {
                            console.debug(fi, firep.itemAt(fi).checked)
                            if (!firep.itemAt(fi).checked)
                                continue
                            let f=(fi)*channelStep+(index+1)
                            console.debug(f, value)
                            dmx.setValue(f, value)
                        }

                        // dmx.setValue(index+1, value)
                    }
                }
            }
        }

    }

    CuteDMX {
        id: dmx
    }

}
