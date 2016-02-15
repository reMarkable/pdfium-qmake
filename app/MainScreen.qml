import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: mainScreen
    signal newNoteClicked
    signal newSketchClicked
    signal archiveClicked
    signal openBook(var path)

    property int smallIconSize: 40
    property int mediumIconSize: 200
    property int bigIconSize: 250

    Item {
        id: quickActions

        anchors {
            top: parent.top
            left: parent.left
            topMargin: 100
            leftMargin: 30
        }

        MainScreenHeader {
            text: "Quick actions"
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
                height: bigIconSize
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
                height: bigIconSize
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
                height: bigIconSize
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
                        mainScreen.archiveClicked(-1)
                    }
                }
            }
        }
    }



    Item {
        id: quickSettings

        anchors {
            topMargin: 100
            leftMargin: 30
            rightMargin: 30
        }

        width: mainScreen.width

        MainScreenHeader {
            text: "Quick settings"
        }

        Grid {
            columns: (rootItem.rotation === 0) ? 1 : 4
            rows: (rootItem.rotation === 0) ? 4 : 1
            id: settingsRow
            height: 100
            anchors.fill: parent
            Repeater {
                id: settingsRepeater
                model: [ {"title": "Lock Device", "icon": "qrc:/icons/Lock_small.svg"},
                    {"title": "Wi-Fi Settings", "icon": "qrc:/icons/Wi-Fi_small.svg"},
                    {"title": "Settings", "icon": "qrc:/icons/settings_small.svg"},
                    {"title": "Setup Guide", "icon": "qrc:/icons/Help_small.svg"},
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
                        height: smallIconSize
                        width: height
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
    }


    Item {
        id: frequentlyUsed
        height: (mediumIconSize + 60) * frequentlyUsedGrid.rows
        width: quickActions.width

        anchors {
            topMargin: 100
            leftMargin: 30
        }

        MainScreenHeader {
            text: "Frequently used"
        }

        Grid {
            id: frequentlyUsedGrid

            anchors.fill: parent
            anchors.topMargin: 10

            columns: 5
            columnSpacing: (parent.width / columns) - mediumIconSize

            onRowsChanged: frequentlyUsedRepeater.model = Collection.recentlyUsedPaths(rows * columns)

            Repeater {
                id: frequentlyUsedRepeater
                model: Collection.recentlyUsedPaths()

                Item {
                    width: mediumIconSize
                    height: frequentlyUsedGrid.height / frequentlyUsedGrid.rows
                    Image {
                        anchors.centerIn: parent
                        height: mediumIconSize
                        width: height
                        source: Collection.thumbnailPath(modelData)
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
                        elide: Text.ElideLeft
                        width: parent.width
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
    }


    Item {
        id: recentlyUsed

        anchors {
            topMargin: 100
            leftMargin: 30
            rightMargin: 30
        }

        MainScreenHeader {
            text: "Recently used"
        }

        Column {
            id: recentlyUsedColumn
            anchors.fill: parent

            Repeater {
                model: [
                    {"title": "Master thesis.pdf", "icon": "qrc:/icons/book.svg"},
                    {"title": "Master notebook.rm", "icon": "qrc:/icons/new note.svg"},
                    {"title": "Master sketchbook.rm", "icon": "qrc:/icons/new sketch.svg"},
                    {"title": "Doodling sketchbook", "icon": "qrc:/icons/new sketch.svg"},
                ]

                Item {
                    width: recentlyUsedColumn.width - 30
                    height: 60

                    Image {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                        }
                        id: recentIcon
                        height: smallIconSize
                        width: height
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
    }



    Item {
        id: recentlyImported

        anchors {
            topMargin: 100
            leftMargin: 30
            rightMargin: 30
        }

        MainScreenHeader {
            text: "Recently imported"
        }

        Column {
            id: recentlyImportedColumn

            Repeater {
                model: [
                    {"title": "Master thesis.pdf", "icon": "qrc:/icons/book.svg"},
                    {"title": "Master notebook.rm", "icon": "qrc:/icons/new note.svg"},
                    {"title": "Master sketchbook.rm", "icon": "qrc:/icons/new sketch.svg"},
                    {"title": "Doodling sketchbook", "icon": "qrc:/icons/new sketch.svg"},
                ]

                Item {
                    width: recentlyUsedColumn.width - 30
                    height: 60

                    Image {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                        }
                        id: recentImportIcon
                        height: smallIconSize
                        width: height
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

    state: (rootItem.rotation === 0) ? "LANDSCAPE" : "PORTRAIT"
    states: [
        State {
            name: "PORTRAIT"
            PropertyChanges { target: quickActions; width: mainScreen.width - 60 }
            PropertyChanges { target: quickSettings; height: smallIconSize }
            PropertyChanges { target: recentlyUsed; height: mainScreen.height / 4}
            PropertyChanges { target: recentlyImported; height: mainScreen.height / 4}
            PropertyChanges { target: frequentlyUsedGrid; rows: 2 }

            AnchorChanges { target: quickSettings; anchors.top: quickActions.bottom }
            AnchorChanges { target: quickSettings; anchors.right: parent.right }
            AnchorChanges { target: quickSettings; anchors.left: parent.left }

            AnchorChanges { target: frequentlyUsed; anchors.top: quickSettings.bottom }
            AnchorChanges { target: frequentlyUsed; anchors.left: parent.left }

            AnchorChanges { target: recentlyUsed; anchors.left: parent.left }
            AnchorChanges { target: recentlyUsed; anchors.right: parent.horizontalCenter }
            AnchorChanges { target: recentlyUsed; anchors.top: frequentlyUsed.bottom }

            AnchorChanges { target: recentlyImported; anchors.right: parent.right }
            AnchorChanges { target: recentlyImported; anchors.left: parent.horizontalCenter }
            AnchorChanges { target: recentlyImported; anchors.top: frequentlyUsed.bottom }
        },
        State {
            name: "LANDSCAPE"
            PropertyChanges { target: quickActions; width: mainScreen.width * 2/3 }
            PropertyChanges { target: quickSettings; height: 200 }
            PropertyChanges { target: recentlyUsed; height: mainScreen.height / 3 }
            PropertyChanges { target: frequentlyUsedGrid; rows: 2 }
            PropertyChanges { target: recentlyImported; height: mainScreen.height / 3 }

            AnchorChanges { target: quickSettings; anchors.top: parent.top }
            AnchorChanges { target: quickSettings; anchors.right: parent.right }
            AnchorChanges { target: quickSettings; anchors.left: quickActions.right }

            AnchorChanges { target: frequentlyUsed; anchors.top: quickActions.bottom }
            AnchorChanges { target: frequentlyUsed; anchors.left: parent.left }

            AnchorChanges { target: recentlyUsed; anchors.left: quickActions.right }
            AnchorChanges { target: recentlyUsed; anchors.right: parent.right }
            AnchorChanges { target: recentlyUsed; anchors.top: quickActions.bottom }

            AnchorChanges { target: recentlyImported; anchors.right: parent.right }
            AnchorChanges { target: recentlyImported; anchors.left: quickActions.right }
            AnchorChanges { target: recentlyImported; anchors.top: frequentlyUsed.verticalCenter }
        }
    ]
}

