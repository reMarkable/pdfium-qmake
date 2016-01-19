import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveView

    signal openBook(var path)

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
            model: Collection.folderEntries()
            interactive: false

            property int selected: -1

            delegate: Rectangle {
                border.width: folderList.selected === index ? 2 : 1
                height: 90
                width: folderlistContainer.width

                Image {
                    id: folderIcon
                    source: Collection.isFolder(modelData) ? "qrc:/icons/folder.png" :"qrc:/icons/book.png"
                    width: 70
                    height: 70
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

                    text: Collection.title(modelData)
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        folderList.selected = index
                        filelistContainer.folderPath = modelData
                    }
                }
            }
        }
    }

    Rectangle {
        id: filelistContainer

        property string folderPath
        onFolderPathChanged: fileList.model = Collection.folderEntries(folderPath)

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
            id: fileList
            anchors.fill: parent
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

                    text: Collection.title(modelData)
                }


                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        archiveView.openBook(modelData)
                    }
                }
            }
        }
    }
}

