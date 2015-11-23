import QtQuick 2.0

Item {
    id: mainScreen
    signal newNoteClicked
    signal newSketchClicked
    signal archiveClicked(var archiveIndex)

    Row {
        width: parent.width
        anchors {
            right: parent.right
            left: parent.left
            top: parent.top
            bottom: archiveHeader.top
        }

        Item {
            width: mainScreen.width / 2
            height: parent.height
            Rectangle {
                id: newNoteItem
                height: mainScreen.width / 3
                width: height
                anchors.centerIn: parent
                border.width: 2
                Text {
                    anchors.centerIn: parent
                    text: "NEW NOTE"
                    color: window.fontColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newNoteClicked()
                }
            }
        }
        Item {
            width: parent.width / 2
            height: parent.height
            Rectangle {
                id: newSketchItem
                height: mainScreen.width / 3
                width: height
                anchors.centerIn: parent
                border.width: 2
                Text {
                    anchors.centerIn: parent
                    text: "NEW SKETCH"
                    color: window.fontColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newSketchClicked()
                }
            }
        }
    }

    Column {
        id: archiveHeader
        anchors {
            left: parent.left
            right: parent.right
            bottom: archiveGrid.top
            leftMargin: 10
        }

        Text {
            color: window.fontColor
            text: "ARCHIVE"
            font.bold: true
        }
        Text {
            color: window.fontColor
            text: "RECENTLY USED"
            font.pointSize: 8
            font.bold: true
        }
    }


    Grid {
        id: archiveGrid
        width: parent.width
        height: width / 2
        anchors {
            left: parent.left
            right: parent.right
            bottom: settingsHeader.top
            bottomMargin: 20
        }

        columns: 4
        columnSpacing: 0

        Repeater {
            model: 7
            Item {
                width: archiveGrid.width / archiveGrid.columns
                height: width
                Rectangle {
                    anchors.centerIn: parent
                    height: mainScreen.width / archiveGrid.columns - 20
                    width: height
                    border.width: 2


                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "DOCUMENT " + modelData
                        color: window.fontColor
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mainScreen.archiveClicked(index)
                        }
                    }
                }
            }
        }

        Item {
            width: archiveGrid.width / archiveGrid.columns
            height: width
            Rectangle {
                id: goToArchiveItem
                height: mainScreen.width / archiveGrid.columns - 20
                width: height
                anchors.centerIn: parent
                color: window.color
                border.width: 1
                Text {
                    anchors.centerIn: parent
                    text: "GO TO ARCHIVE"
                    color: window.fontColor
                    horizontalAlignment: Text.AlignHCenter
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mainScreen.archiveClicked(-1)
                    }
                }
            }
        }
    }

    Text {
        id: settingsHeader
        color: window.fontColor
        text: "SETTINGS"
        font.bold: true

        anchors {
            leftMargin: 10
            bottom: settingsList.top
            left: parent.left
        }
    }

    Row {
        id: settingsList

        width: parent.width
        height: width / 4
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Repeater {
            model: ["PREFERENCES", "MY XO", "NOTIFICATIONS", "IN OUT"]

            Item {
                width: parent.width / 4
                height: parent.height
                Rectangle {
                    id: preferencesItem
                    height: mainScreen.width / 4 - 20
                    width: height
                    anchors.centerIn: parent
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: window.fontColor
                    }
                }
            }
        }

    }
}

