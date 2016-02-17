import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveView

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

    Rectangle {
        anchors.fill: editActionsRow
        color: "#666"
        visible: editActionsRow.visible
        radius: 5
    }

    Row {
        id: editActionsRow
        anchors {
            verticalCenter: editButton.verticalCenter
            right: editButton.left
            rightMargin: 10
        }
        visible: (mainArchive.visible) ? mainArchive.selectionModeActive : archiveBook.selectionModeActive

        ArchiveButton {
            color: "#666"
            icon: "qrc:///icons/Delete_white.svg"
            onClicked: {
                if (mainArchive.visible) {
                    mainArchive.deleteBooks()
                } else {
                    archiveBook.deletePages()
                }
            }
        }
    }

    ArchiveButton {
        id: editButton
        anchors {
            top: parent.top
            right: modeSelect.left
            topMargin: 25
            rightMargin: 10
        }
        color: "#666"
        icon: editActionsRow.visible ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white.svg"
        onClicked: {
            if (mainArchive.visible) {
                mainArchive.selectionModeActive = !mainArchive.selectionModeActive
            } else {
                archiveBook.selectionModeActive = !archiveBook.selectionModeActive
            }
        }
    }

    Rectangle {
        id: modeSelect
        anchors {
            top: parent.top
            right: parent.right
            topMargin: 25
            rightMargin: 50
        }

        width: 150
        height: 75
        color: "#666"
        radius: 5

        ArchiveButton {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            color: "#666"
            icon: "qrc:///icons/Grid 9.svg"
        }

        ArchiveButton {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            color: "#666"
            icon: "qrc:///icons/Grid 36_white.svg"
        }
    }

    ArchiveMain {
        id: mainArchive

        anchors {
            top: archiveHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
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
}
