import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.14
import BackEnd 1.0

ApplicationWindow {
    id: applicationWindow
    width: 640
    height: 480
    visible: true
    title: qsTr("Spread Lights")

    Controls {
        id: controls
        objectName: "controls"
    }

    Row {
        id: header
        height: 50
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        ToolBar {
            id: toolBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            ToolButton {
                id: uploadLMSButton
                x: 10
                y: 0
                objectName: "buttonTest"
                text: qsTr("Upload Channels")
                font.styleName: "Regular"
                font.family: "Arial"
                onClicked: controls.readChannelsFromLMS()
            }

            ToolButton {
                id: uploadModelsButton
                x: 185
                y: 0
                text: qsTr("Upload Models")
                font.family: "Arial"
                font.styleName: "Regular"
                onClicked: controls.loadModelsFromFile()
            }

            ToolButton {
                id: uploadSequenceButton
                x: 360
                y: 0
                text: qsTr("Upload Sequence")
                font.styleName: "Regular"
                font.family: "Arial"
                onClicked: controls.readSequenceFromLMS()
            }


        }
    }

    Row {
        id: mainArea
        height: 429
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: header
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        Column {
            id: channelListColumn
            width: 300
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            ScrollView {
                id: channelListScroll
                x: 0
                y: 60
                width: parent.width
                height: parent.height

                ListView {
                    id: channelListView
                    width: parent.width
//                    model: 20
                    model: controls.channels
                    property var numChecked: numCheckedFunc()
                    function numCheckedFunc() {
                        var count = 0;
                        for (var i = 0; i < children.length; i++) {
                            if (children[i].isChecked) {
                                count += children[i].modelData.isChecked
                            }
                        }
                        return count
                    }
                    delegate: ItemDelegate {
                        CheckBox {
                            text: modelData.name
                            width: 175
                            id: check
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            checked: modelData.isChecked

                            onClicked: {
                                if (modelData.isChecked) {
                                    controls.uncheckChannel(index)
                                    controls.setChaseIndex(index, 0)
                                } else {
                                    controls.setChaseIndex(index, controls.numCheckedChannels + 1)
                                    controls.checkChannel(index)
                                }
                                modelData.setIsChecked(!modelData.isChecked)
                            }

                        }

                        SpinBox {
                            id: orderSpinBox
                            width: 125
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            font.weight: Font.Black
                            font.pointSize: 16
                            value: modelData.chaseIndex
//                            onValueChanged: modelData.chaseIndex = value;
//                            onValueChanged: controls.setChaseIndex(index, value)
                        }
                        width: channelListView.width
                    }
                }
            }
        }

        Column {
            id: controlsColumn
            x: 328
            width: 340
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: channelListColumn

            TextInput {
                id: nameTextInput
                y: 56
                width: 200
                height: 20
                anchors.left: parent.left
                anchors.leftMargin: 10
//                placeholderText: qsTr("Model name")
                text: controls.name
                onEditingFinished: controls.name = text
                font.pixelSize: 18
            }

            RadioButton {
                id: parentRadioButton
                anchors.left: parent.left
                anchors.leftMargin: 10
                checked: controls.isParent
                onToggled: () => {
                               childSelect.enabled = false
                               modelSelect.enabled = true
                               controls.isParent = true
                           }
                y: 100
                text: qsTr("Parent")
            }

            RadioButton {
                id: childRadioButton
                anchors.left: parent.left
                anchors.leftMargin: 10
                checked: !controls.isParent
                onToggled: () => {
                               childSelect.enabled = true
                               modelSelect.enabled = false
                               controls.isParent = false
                           }
                y: 209
                text: qsTr("Child")
            }

            ComboBox {
                id: modelSelect
                objectName: "modelSelect"
                y: 300
                width: 200
                anchors.left: parent.left
                anchors.leftMargin: 10

                model: controls.parents
//                onClick: console.log(controls.getInstance().parents)
                textRole: "name"
                ScrollBar.vertical: ScrollBar {
                    parent: modelSelect.parent
//                    anchors.top: modelSelect.top
//                    anchors.left: modelSelect.right
//                    anchors.bottom: modelSelect.bottom
                }
            }

            ComboBox {
                id: childSelect
                objectName: "childSelect"
                y: 400
                width: 200
                anchors.left: parent.left
                anchors.leftMargin: 10

                model: controls.children
//                onClick: console.log(controls.getInstance().parents)
                textRole: "name"
                ScrollBar.vertical: ScrollBar {
                    parent: childSelect.parent
//                    anchors.top: modelSelect.top
//                    anchors.left: modelSelect.right
//                    anchors.bottom: modelSelect.bottom
                }
                enabled: !controls.isParent
            }

            Row {
                y: 550
                width: parent.width
                height: 100

                Button {
                    id: newModelButton
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    text: qsTr("New Model")
                    onClicked: controls.saveAndNewModel()
                }

                Button {
                    id: deleteModelButton
                    anchors.left: newModelButton.right
                    anchors.leftMargin: 10
                    text: qsTr("Delete Model")
                    onClicked: controls.deleteModel()
                }

                Button {
                    id: saveButton
                    anchors.left: deleteModelButton.right
                    anchors.leftMargin: 10
                    text: qsTr("Save")
                    onClicked: controls.saveModel()
                }

            }
            Button {
                id: spreadSequenceButton
                x: 500
                height: 50
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 50
                text: qsTr("Spread Sequence")
                onClicked: controls.spreadSequence()
            }
        }
    }
}
