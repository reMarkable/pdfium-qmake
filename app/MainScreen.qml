import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: mainScreen
    signal newNoteClicked
    signal newSketchClicked
    signal archiveClicked
    signal openBook(var path)

    Text {
        anchors {
            left: parent.left
            right: parent.right
            bottom: newSketchItem.top
            bottomMargin: 10
            leftMargin: 30
        }
        color: window.fontColor
        text: "Quick actions"
        font.pointSize: 25
        font.bold: true
    }

    Rectangle {
        anchors {
            right: parent.right
            left: parent.left
            bottom: newSketchItem.top
            leftMargin: 30
            rightMargin: 30
        }
        height: 3
        color: "gray"
    }


    Item {
        id: newNoteItem
        height: mainScreen.width / 4
        width: height
        anchors {
            top: parent.top
            topMargin: 120
            right: newSketchItem.left
        }
        Image {
            source: "qrc:/icons/new note-large.svg"
            anchors.fill: parent
            anchors.margins: 30
            smooth: true

            Text {
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 18
                text: "New note"
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mainScreen.newNoteClicked()
        }
    }

    Item {
        id: newSketchItem
        height: mainScreen.width / 4
        width: height
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 120
        }

        Image {
            source: "qrc:/icons/new sketch-large.svg"
            anchors.fill: parent
            anchors.margins: 30
            smooth: true

            Text {
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                text: "New sketch"
                font.pointSize: 18
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mainScreen.newSketchClicked()
        }
    }

    Item {
        id: goToArchiveItem
        height: mainScreen.width / 4
        width: height
        anchors {
            top: parent.top
            topMargin: 120
            left: newSketchItem.right
        }

        Image {
            source: "qrc:/icons/Archive-large.svg"
            anchors.fill: parent
            anchors.margins: 30
            smooth: true

            Text {
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                font.pointSize: 18
                text: "Archive"
            }
        }


        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainScreen.archiveClicked(-1)
            }
        }
    }

    Text {
        id: settingsHeader
        anchors {
            left: parent.left
            right: parent.right
            bottom: settingsRow.top
            bottomMargin: 30
            leftMargin: 30
        }
        color: window.fontColor
        text: "Quick settings"
        font.pointSize: 25
        font.bold: true

        Image {
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width: height
            source: "qrc:/icons/forward.svg"
        }
    }

    Rectangle {
        anchors {
            right: parent.right
            left: parent.left
            bottom: settingsRow.top
            leftMargin: 30
            rightMargin: 30
            bottomMargin: 20
        }
        height: 3
        color: "gray"
    }

    Row {
        id: settingsRow
        height: 100
        anchors.top: newSketchItem.bottom
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.right: parent.right
        anchors.topMargin: 150
        Repeater {
            id: settingsRepeater
            model: [ {"title": "Lock Device", "icon": "qrc:/icons/x-out.svg"},
                {"title": "Wi-Fi Settings", "icon": "qrc:/icons/focus+.svg"},
                {"title": "Settings", "icon": "qrc:/icons/list.svg"},
                {"title": "Setup Guide", "icon": "qrc:/icons/help.svg"},
            ]

            Item {
                width: settingsRow.width / 5
                height: 50

                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    id: icon
                    height: 30
                    width: 30
                    source: modelData.icon
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        leftMargin: 10
                        left: icon.right
                    }

                    text: modelData.title
                }
            }
        }
    }

    Text {
        id: archiveHeader
        anchors {
            left: parent.left
            right: parent.right
            bottom: archiveGrid.top
            bottomMargin: 30
            leftMargin: 30
        }
        color: window.fontColor
        text: "Frequently used"
        font.pointSize: 25
        font.bold: true

        Image {
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width: height
            source: "qrc:/icons/forward.svg"
        }
    }

    Rectangle {
        anchors {
            right: parent.right
            left: parent.left
            bottom: archiveGrid.top
            leftMargin: 30
            rightMargin: 30
            bottomMargin: 20
        }
        height: 3
        color: "gray"
    }

    Grid {
        id: archiveGrid
        height: archiveGrid.width / archiveGrid.columns
        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            top: settingsRow.bottom
            topMargin: 100
        }

        columns: 4
        columnSpacing: 0

        Repeater {
            model: Collection.recentlyUsedPaths()
            Item {
                width: archiveGrid.width / archiveGrid.columns
                height: width
                Image {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    //anchors.centerIn: parent
                    height: parent.width - 50
                    width: height
                    source: Collection.thumbnailPath(modelData) === "" ? "" : "file://" + Collection.thumbnailPath(modelData)
                    fillMode: Image.PreserveAspectCrop

                    Image {
                        anchors.fill: parent
                        source: "qrc:/icons/clear page-thumb.svg"
                    }
                }

                Text {
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: Collection.title(modelData)
                    color: window.fontColor
                    font.pointSize: 18
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

    Text {
        anchors {
            left: parent.left
            right: recentlyUsedColumn.right
            bottom: recentlyUsedColumn.top
            bottomMargin: 30
            leftMargin: 30
        }
        color: window.fontColor
        text: "Recently edited"
        font.pointSize: 25
        font.bold: true

        Image {
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width: height
            source: "qrc:/icons/forward.svg"
        }
    }

    Rectangle {
        anchors {
            right: recentlyUsedColumn.right
            left: parent.left
            bottom: recentlyUsedColumn.top
            leftMargin: 30
            rightMargin: 30
            bottomMargin: 20
        }
        height: 3
        color: "gray"
    }

    Column {
        id: recentlyUsedColumn
        anchors.top: archiveGrid.bottom
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.right: parent.horizontalCenter
        anchors.topMargin: 150
        Repeater {
            model: [
                {"title": "Master thesis.pdf", "icon": "qrc:/icons/book.svg"},
                {"title": "Master notebook.rm", "icon": "qrc:/icons/new note.svg"},
                {"title": "Master sketchbook.rm", "icon": "qrc:/icons/new sketch.svg"},
                {"title": "Doodling sketchbook", "icon": "qrc:/icons/new sketch.svg"},
            ]

            Item {
                width: recentlyUsedColumn.width - 30
                height: 50

                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    id: recentIcon
                    height: 30
                    width: 30
                    source: modelData.icon
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        leftMargin: 10
                        left: recentIcon.right
                    }

                    text: modelData.title
                }

                Rectangle {
                    width: parent.width
                    height: 2
                    color: "gray"
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    Text {
        anchors {
            left: recentlyImportedColumn.left
            right: parent.right
            bottom: recentlyImportedColumn.top
            bottomMargin: 30
            rightMargin: 30
        }
        color: window.fontColor
        text: "Recently imported"
        font.pointSize: 25
        font.bold: true

        Image {
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            width: height
            source: "qrc:/icons/forward.svg"
        }
    }

    Rectangle {
        anchors {
            left: recentlyImportedColumn.left
            right: parent.right
            bottom: recentlyImportedColumn.top
            rightMargin: 60
            bottomMargin: 20
        }
        height: 3
        color: "gray"
    }

    Column {
        id: recentlyImportedColumn
        anchors.top: archiveGrid.bottom
        anchors.right: parent.left
        anchors.rightMargin: 30
        anchors.left: parent.horizontalCenter
        anchors.topMargin: 150
        Repeater {
            model: [
                {"title": "Master thesis.pdf", "icon": "qrc:/icons/book.svg"},
                {"title": "Master notebook.rm", "icon": "qrc:/icons/new note.svg"},
                {"title": "Master sketchbook.rm", "icon": "qrc:/icons/new sketch.svg"},
                {"title": "Doodling sketchbook", "icon": "qrc:/icons/new sketch.svg"},
            ]

            Item {
                width: recentlyUsedColumn.width - 30
                height: 50

                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    id: recentImportIcon
                    height: 30
                    width: 30
                    source: modelData.icon
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        leftMargin: 10
                        left: recentImportIcon.right
                    }

                    text: modelData.title
                }

                Rectangle {
                    width: parent.width
                    height: 2
                    color: "gray"
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

}

