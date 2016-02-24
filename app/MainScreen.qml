import QtQuick 2.0
import com.magmacompany 1.0

Item  {
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
            right: parent.right
            bottom: frequentlyUsed.top
            bottomMargin: 25
        }

        Item {
            id: newNoteItem

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: parent.horizontalCenter
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

        Item {
            anchors {
                right: parent.right
                left: parent.horizontalCenter
                top: parent.top
                bottom: parent.verticalCenter
            }

            Image {
                source: "qrc:/icons/Notebook.svg"
                anchors.centerIn: parent
                height: mediumIconSize
                width: height
                smooth: true

                Image {
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }
                    source: "qrc:/icons/pluss.svg"
                    width: smallIconSize
                    height: width
                }

                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "New notebook"
                    font.pixelSize: 25
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newSketchClicked()
                }
            }
        }

        Item {
            anchors {
                right: parent.right
                left: parent.horizontalCenter
                bottom: parent.bottom
                top: parent.verticalCenter
            }

            Image {
                source: "qrc:/icons/Notebook.svg"
                anchors.centerIn: parent
                height: mediumIconSize
                width: height
                smooth: true

                Image {
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }
                    source: "qrc:/icons/pluss.svg"
                    width: smallIconSize
                    height: width
                }

                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "New sketchbook"
                    font.pixelSize: 25
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainScreen.newSketchClicked()
                }
            }
        }
    }


    MainScreenPreviews {
        id: frequentlyUsed

        anchors {
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 20
            bottom: logo.top
            bottomMargin: 50
        }

        rows: 3
        columns: 4
        title: "Frequently used"

        function reloadModel() {
            model = Collection.getFrequentlyOpenedPaths(rows * columns)
        }

        Component.onCompleted: reloadModel()
        Connections {
            target: Collection
            onDocumentsOpenCountsChanged: {
                frequentlyUsed.reloadModel()
            }
        }
        onOpenClicked: mainScreen.archiveClicked()
    }

    Image {
        id: logo
        anchors {
            bottom: parent.bottom
            bottomMargin: 15
            horizontalCenter: parent.horizontalCenter
        }
        height: 25
        width: height * 6.2
        source: "qrc:/icons/reMarkable.svg"
    }
}

