import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveView

    signal openBook(var path)

    Rectangle {
        id: folderlistContainer
        width: parent.width / 2 - 40
        anchors {
            top: parent.top
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
                    source: Collection.isFolder(modelData) ? "qrc:/icons/Full folder.svg" :"qrc:/icons/book.svg"
                    width: 70
                    height: 70
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: 15
                    }
                }

                Text {
                    anchors {
                        left: folderIcon.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }

                    font.pointSize: 18
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
            top: parent.top
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
                    source: "qrc:/icons/book.svg"
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }
                    width: 70
                    height: 70
                }

                Text {
                    anchors {
                        left: bookIcon.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5
                    }
                    font.pointSize: 18

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

