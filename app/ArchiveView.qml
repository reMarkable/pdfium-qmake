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

        width: 325
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
            font.pointSize: 15
            color: "white"
            text: "Main archive"
            horizontalAlignment: Text.AlignHCenter
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

        width: 325
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
            font.pointSize: 15
            color: "white"
            text: Collection.title(archiveView.currentBook)
            horizontalAlignment: Text.AlignHCenter
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
        visible: false

        Item {
            height: 75
            width: height
            Image {
                anchors {
                    fill: parent
                    margins: 10
                }

                source: "qrc:///icons/Move_white.svg"
            }
        }

        Item {
            height: 75
            width: height
            Image {
                anchors {
                    fill: parent
                    margins: 10
                }

                source: "qrc:///icons/Delete_white.svg"
            }
        }
        Item {
            height: 75
            width: height
            Image {
                anchors {
                    fill: parent
                    margins: 10
                }

                source: "qrc:///icons/send_white.svg"
            }
        }
    }

    Rectangle {
        anchors {
            verticalCenter: editButton.verticalCenter
            right: editButton.left
            rightMargin: -width/2
        }
        width: 15
        height: width
        color: "#666"
        rotation: 45
        visible: editActionsRow.visible
    }

    Rectangle {
        id: editButton
        anchors {
            top: parent.top
            right: modeSelect.left
            topMargin: 25
            rightMargin: 10
        }

        width: height
        height: 75
        color: "#666"
        radius: 5

        Image {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 10
            }
            width: height
            source: editActionsRow.visible ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white.svg"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: editActionsRow.visible = !editActionsRow.visible
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

        Image {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 10
            }
            width: height
            source: "qrc:///icons/Grid 9.svg"
        }

        Image {
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
                margins: 10
            }
            width: height
            source: "qrc:///icons/Grid 36_white.svg"
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
