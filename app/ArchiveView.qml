import QtQuick 2.0

Rectangle {
    id: archiveView

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

    ListModel {
        id: folders
        ListElement {
            name: "DROPBOX"
            files: [
                ListElement { name: "DRAWING"},
                ListElement { name: "NOTE"},
                ListElement { name: "DOCUMENT"}
            ]
        }
        ListElement {
            name: "LOCAL"
            files: [
                ListElement { name: "DOCUMENT LOL"},
                ListElement { name: "DOCUMENT KEK"},
                ListElement { name: "NOTE KKEKEKE"}
            ]
        }
    }

    Rectangle {
        id: folderlistContainer
        width: parent.width / 4
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
            model: folders

            property int selected: -1

            delegate: Rectangle {
                border.width: folderList.selected === index ? 2 : 1
                height: 40
                width: folderlistContainer.width

                Text {
                    anchors {
                        left: parent.left
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
                    console.log(name)
                }
            }
        }
    }

    Rectangle {
        id: filelistContainer
        width: parent.width / 4
        border.width: 0
        anchors {
            top: folderlistContainer.top
            left: folderlistContainer.right
            bottom: folderlistContainer.bottom
            leftMargin: 5
        }

        ListView {
            anchors.fill: parent
            id: fileList

            spacing: 5

            model: []
            delegate: Rectangle {
                border.width: 1
                height: 40
                width: folderlistContainer.width

                Text {
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }

                    text: modelData
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}

