import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

import org.tal.dmx

ApplicationWindow {
    width: 800
    height: 480
    visible: true
    title: qsTr("Cute DMX")

    header: ToolBar {
        RowLayout {
            ToolButton {
                text: "Quit"
                onClicked: Qt.exit(0)
            }
            ToolButton {
                text: "Open"
                onClicked: {
                    dmx.start();
                }
            }
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

        RowLayout {
            spacing: 16
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            Repeater {
                model: 8
                ChannelSlider {
                    channelLabel: index+1
                    onValueChanged: {
                        for (let fi=0;fi<firep.count;fi++) {
                            console.debug(fi, firep.itemAt(fi).checked)
                            if (!firep.itemAt(fi).checked)
                                continue
                            let f=(fi)*8+(index+1)
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
