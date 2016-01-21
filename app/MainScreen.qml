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
        font.pointSize: 8
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
        font.pointSize: 8
        font.bold: true
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
            top: newSketchItem.bottom
            topMargin: 150
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
                    height: parent.width - 20
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
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mainScreen.openBook(modelData)
                    }
                }
            }
        }

        Item {
            width: archiveGrid.width / archiveGrid.columns
            height: width
        }
    }
}

