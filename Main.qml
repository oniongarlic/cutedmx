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
    minimumWidth: 800
    minimumHeight: 480
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
        id: settingsChannels
        category: "channels"
    }

    Settings {
        id: settingsDmx
        category: "dmx"
    }

    DmxModel {
        id: dmxModel
    }

    Shortcut {
        sequences: [StandardKey.FullScreen]
        onActivated: menuFullScreen.click()
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
            console.debug("Using serial", device)
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
                text: "Disconnect"
                enabled: dmx.active
                onTriggered: {
                    dmx.stop();
                }
            }
            MenuSeparator {

            }
            MenuItem {
                id: menuDemoMode
                text: "&Demo mode"
                checkable: true
                checked: false
                enabled: !dmx.active
            }

            MenuSeparator {

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

            MenuItem {
                id: menuFullScreen
                text: "&Full screen"
                checkable: true
                checked: visibility==Window.FullScreen ? true : false
                onCheckedChanged: visibility=!checked ? Window.Windowed : Window.FullScreen
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
            MenuSeparator {

            }
            MenuItem {
                id: showButtonsMenu
                checkable: true
                checked: true
                text: "Show buttons"
            }
        }

        Menu {
            title: "Scenes"
            MenuItem {
                text: "Play"
                enabled: !sceneTicker.running && dmxModel.count>0
                onClicked: startTicker();
            }
            MenuItem {
                text: "Stop"
                enabled: sceneTicker.running
                onClicked: stopTicker();
            }
            MenuSeparator {

            }
            MenuItem {
                text: "Clear"
                onClicked: dmxModel.clear();
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

    function updateChannelsGrid(gs) {
        switch (gs) {
        case '8': {
            app.channels=8
            app.split=1
        }
        break;
        case '16': {
            app.channels=16
            app.split=2
        }
        break;
        case '32': {
            app.channels=32
            app.split=2
        }
        break;
        }
        settingsChannels.setValue('gs', gs)
    }

    Timer {
        id: sceneTicker
        repeat: true
        interval: sceneSpeed.value
        property int row: 1

        onTriggered: {
            row++;
            if (row>dmxModel.count)
                row=1

            setDmxFromRow(row);

            lv.currentIndex=row-1;
        }
    }

    function startTicker() {
        sceneTicker.start()
    }

    function stopTicker() {
        sceneTicker.stop()
    }

    function setDmxFromRow(r) {
        console.debug("Set", r)
        for (let c=1;c<512;c++) {
            let v=dmxModel.dmxValue(r, c)
            dmx.setValue(c, v, true)
        }
        dmx.updateFrame();
    }

    ButtonGroup {
        id: channelsGroup
        onClicked: {
            updateChannelsGrid(button.objectName);
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
            enabled: dmx.active || menuDemoMode.checked
            rows: split
            columns: channels/rows
            uniformCellWidths: true
            Repeater {
                id: sliderRepeater
                model: channels
                ChannelSlider {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    showButtons: showButtonsMenu.checked
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



        ColumnLayout {

            Label {
                text: lv.currentIndex + "/" + dmxModel.count
            }

            ListView {
                id: lv
                Layout.minimumWidth: 100
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: dmxModel
                delegate: ItemDelegate {
                    required property int index;
                    required property string name;
                    required property int delay;
                    highlighted: ListView.isCurrentItem
                    text: name
                    onDoubleClicked: {
                        console.debug(name, delay, index)
                        setDmxFromRow(index);
                    }
                    onClicked: {
                        lv.currentIndex=index
                    }
                }
            }

            Label {
                text: sceneSpeed.value+"ms"
            }

            RowLayout {
                Button {
                    text: "+"
                    onClicked: {
                        let r=dmxModel.addRow("Scene", sceneSpeed.value);
                        for (let c=1;c<512;c++) {
                            dmxModel.setDmxValue(r, c, dmx.value(c))
                        }
                    }
                }
                Button {
                    text: "-"
                    enabled: dmxModel.count>0 && lv.currentIndex>-1
                    onClicked: dmxModel.removeRow(lv.currentIndex)
                }
                Button {
                    text: "="
                    enabled: dmxModel.count>0 && lv.currentIndex>-1
                    onClicked: {
                        for (let c=1;c<512;c++) {
                            dmxModel.setDmxValue(lv.currentIndex+1, c, dmx.value(c))
                        }
                    }
                }
            }

        }

        Slider {
            id: sceneSpeed
            Layout.fillHeight: true
            from: 100
            to: 4000
            value: 1000
            stepSize: 100
            snapMode: Slider.SnapAlways
            orientation: Qt.Vertical
            wheelEnabled: true
        }

    }

    CuteDMX {
        id: dmx
    }

}
