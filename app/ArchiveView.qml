import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveView

    property string title: "Archive"

    signal openBook(var path)
    signal openBookAt(var path, var page)

    property string currentBook: ""

    Rectangle {
        anchors {
            verticalCenter: archiveHeader.verticalCenter
            left: archiveHeader.right
            leftMargin: -15
        }

        width: 30
        height: 30
        color: archiveHeader.color
        rotation: 45
        visible: bookHeader.visible
    }

    Rectangle {
        id: bookHeader
        anchors {
            top: parent.top
            left: archiveHeader.right
            topMargin: 25
            leftMargin: 30
        }

        width: 250
        height: 75
        color: "#666"
        radius: 5
        visible: archiveView.currentBook !== ""

        Image {
            id: bookHeaderIcon
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 20
            }
            width: height

            source: "qrc:///icons/Notebook_white.svg"

        }

        Text {
            anchors {
                left: bookHeaderIcon.right
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            font.bold: true
            font.pixelSize: 25
            color: "white"
            text: Collection.title(archiveView.currentBook)
        }
    }

    ArchiveMain {
        id: mainArchive

        anchors.fill: parent
    }

    ArchiveBook {
        id: archiveBook

        anchors {
            top: archiveHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        id: archiveHeader
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 25
            leftMargin: 50
        }

        width: 250
        height: 75
        color: bookHeader.visible ? "#aaa" : "#666"
        radius: 5

        Image {
            id: archiveHeaderIcon
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 20
            }
            width: height

            source: "qrc:///icons/Archive-small_white.svg"
        }

        Text {
            anchors {
                left: archiveHeaderIcon.right
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            font.bold: true
            font.pixelSize: 25
            color: "white"
            text: "Main archive"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: archiveView.currentBook = ""
        }
    }
}
