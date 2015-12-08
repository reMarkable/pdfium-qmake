import QtQuick 2.0

Rectangle {
    id: archiveView

    signal openBook(var name, var files)

    property var folderModel

    Text {
        id: header
        text: "ARCHIVE"
        font.pointSize: 20
        anchors {
            margins: 20
            top: parent.top
            left: parent.left
        }
    }

    Rectangle {
        id: folderlistContainer
        width: parent.width / 2 - 40
        anchors {
            top: header.bottom
            left: parent.left
            bottom: parent.bottom
            topMargin: 20
            leftMargin: 20
        }

        border.width: 0
        ListView {
            id: folderList
            anchors.fill: parent

            spacing: 5
            model: archiveView.folderModel
            interactive: false

            property int selected: -1

            delegate: Rectangle {
                border.width: folderList.selected === index ? 2 : 1
                height: 90
                width: folderlistContainer.width

                Image {
                    id: folderIcon
                    source: "qrc:/icons/folder.png"
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }
                }

                Text {
                    anchors {
                        left: folderIcon.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }

                    text: name
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        folderList.selected = index
                        fileList.model = files
                    }
                }
                Component.onCompleted: {
                    console.log("Created for: " + name)
                }
            }
        }
    }

    Rectangle {
        id: filelistContainer
        width: parent.width / 2 - 40
        border.width: 0
        anchors {
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
            topMargin: 20
            rightMargin: 20
        }

        ListView {
            anchors.fill: parent
            id: fileList
            interactive: false

            spacing: 5

            model: []
            delegate: Rectangle {
                border.width: 1
                height: 90
                width: folderlistContainer.width

                Image {
                    id: bookIcon
                    source: "qrc:/icons/book.png"
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }
                    height: parent.height - 5
                    width: height
                }

                Text {
                    anchors {
                        left: bookIcon.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }

                    text: name
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        archiveView.openBook(name, files)
                    }
                }
            }
        }
    }
}

