import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveView

    property string title: "Archive"
    property string tabIdentifier: "archive"

    signal openBook(var path)
    signal openBookAt(var path, var page)

    property string currentBook: ""
    onCurrentBookChanged: {
        if (currentBook === "") {
            if (archiveBook.document) {
                archiveBook.document.releaseWorker()
            }

            archiveBook.document = null
        } else {
            archiveBook.document = Collection.getDocument(archiveView.currentBook)
            archiveBook.document.acquireWorker()
        }
    }

    Component.onDestruction:{
        if (archiveBook.document) {
            archiveBook.document.releaseWorker()
        }
    }

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

        width: 450
        height: 75
        color: "#666"
        radius: 5
        visible: archiveView.currentBook !== ""

        Image {
            id: bookHeaderIcon
            anchors {
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }

            width: height
            height: parent.height - 20
            sourceSize.width: width
            sourceSize.height: height
            asynchronous: true

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
            text: archiveBook.document === null ? "" : archiveBook.document.title
        }

        ArchiveButton {
            id: showBookButton
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
            }
            color: "transparent"
            width: 75
            icon: "qrc:///icons/Open-book_white.svg"
            onClicked: archiveView.openBook(archiveView.currentBook)
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
                left: parent.left
                leftMargin: 20
                verticalCenter: parent.verticalCenter
            }
            asynchronous: true

            height: parent.height - 20
            width: height
            sourceSize.width: width
            sourceSize.height: height

            source: "qrc:///icons/Archive-small_white.svg"
        }

        Text {
            anchors {
                left: archiveHeaderIcon.right
                leftMargin: 20
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            font.bold: true
            font.pixelSize: 25
            color: "white"
            text: "Archive"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: archiveView.currentBook = ""
        }
    }

    ArchiveMain {
        id: mainArchive

        anchors.fill: parent
    }

    BookOverview {
        id: archiveBook

        visible: archiveView.currentBook != ""

        onVisibleChanged: {
            if (visible) {
                forceActiveFocus()
            }
        }
        onPageClicked: archiveView.openBookAt(archiveView.currentBook, index)

        anchors.fill: parent
    }
}
