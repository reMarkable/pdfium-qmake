import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: mainScreen
    signal newNoteClicked
    signal newSketchClicked
    signal archiveClicked
    signal openBook(var path)

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
                Image {
                    source: "qrc:/icons/new note-large.svg"
                    anchors.fill: parent
                    anchors.margins: 40
                    smooth: true
                    //opacity: 0.5
                }

                Image {
                    source: "qrc:/icons/pluss.svg"
                    width: parent.width / 3
                    height: width
                    anchors.left: parent.left
                    anchors.top: parent.top
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
                Image {
                    source: "qrc:/icons/new sketch-large.svg"
                    anchors.fill: parent
                    anchors.margins: 40
                    smooth: true
                }

                Image {
                    source: "qrc:/icons/pluss.svg"
                    width: parent.width / 3
                    height: width
                    anchors.left: parent.left
                    anchors.top: parent.top
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
            leftMargin: 20
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
        height: archiveGrid.width / archiveGrid.columns
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 20
        }

        columns: 4
        columnSpacing: 0

        Repeater {
            model: Collection.recentlyUsedPaths()
            Item {
                width: archiveGrid.width / archiveGrid.columns
                height: width
                Rectangle {
                    anchors.centerIn: parent
                    height: mainScreen.width / archiveGrid.columns - 20
                    width: height
                    border.width: 2

                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: Collection.thumbnailPath(modelData) === "" ? "qrc:/icons/clear page.svg" : "file://" + Collection.thumbnailPath(modelData)
                        fillMode: Image.PreserveAspectCrop
                        Rectangle {
                            anchors.fill: parent
                            color: "#8fffffff"
                        }
                    }

                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: Collection.title(modelData)
                        color: window.fontColor
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mainScreen.openBook(modelData)
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
                    text: "GO TO ARCHIVE ->"
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

    /*Text {
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
            model: ["PREFERENCES", "MY XO", "NOTIFICATIONS", "IN/OUT"]

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

    }*/
}

