import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: mainScreen
    signal newNoteClicked
    signal newSketchClicked
    signal archiveClicked
    signal openBook(var path)

    property int smallIconSize: 40
    property int mediumIconSize: 100
    property int bigIconSize: 200
    property int hugeIconSize: 250

    Item {
        id: quickActions

        anchors {
            top: parent.top
            left: parent.left
            topMargin: 100
            leftMargin: 30
        }

        height: 300
        width: parent.width

        Item {
            id: newNoteItem

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: newSketchItem.left
            }

            Image {
                source: "qrc:/icons/new note-large.svg"
                anchors.centerIn: parent
                height: hugeIconSize
                width: height
                smooth: true

                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 18
                    text: "New note"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newNoteClicked()
                }
            }
        }

        Item {
            id: newSketchItem

            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }

            Image {
                source: "qrc:/icons/new sketch-large.svg"
                anchors.centerIn: parent
                height: hugeIconSize
                width: height
                smooth: true

                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "New sketch"
                    font.pointSize: 18
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newSketchClicked()
                }
            }

        }

        Item {
            id: goToArchiveItem

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: newSketchItem.right
                right: parent.right
            }

            Image {
                source: "qrc:/icons/Archive-large.svg"
                anchors.centerIn: parent
                height: hugeIconSize
                width: height
                smooth: true

                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 20
                    font.pointSize: 18
                    text: "Archive"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mainScreen.archiveClicked()
                    }
                }
            }
        }
    }


    MainScreenPreviews {
        id: frequentlyUsed

        anchors {
            left: parent.left
            right: parent.right
            bottom: recentlyImported.top
        }

        rows: 2
        columns: 4
        title: "Frequently used"

        function reloadModel() {
            model = Collection.recentlyUsedPaths(rows * columns)
        }

        Component.onCompleted: reloadModel()
        Connections {
            target: Collection
            onRecentlyUsedChanged: {
                frequentlyUsedGrid.reloadModel()
            }
        }
    }

    MainScreenPreviews {
        id: recentlyImported

        anchors {
            left: parent.left
            right: parent.right
            bottom: logoText.top
            bottomMargin: 20
        }

        rows: 1
        columns: 4
        title: "Recently imported"

        function reloadModel() {
            model = Collection.recentlyImportedPaths(rows * columns)
        }

        Component.onCompleted: reloadModel()
        Connections {
            target: Collection
            onRecentlyUsedChanged: {
                frequentlyUsedGrid.reloadModel()
            }
        }
    }

    Text {
        id: logoText
        anchors {
            bottom: parent.bottom
            bottomMargin: 20
            left: parent.left
            right: parent.right
        }
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 24
        text: "reMarkable"
    }
}

