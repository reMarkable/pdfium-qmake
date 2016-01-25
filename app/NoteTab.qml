import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    property int iconMargin: 8

//    Column {
//        anchors.fill: parent
//        anchors.topMargin: 75
//        anchors.leftMargin: 20
//        spacing: 50
//        Repeater {
//            model: 32
//            Rectangle {
//                width: parent.width - 30
//                height: 1
//                color: "#321123"
//            }
//        }
//    }

//    Rectangle {
//        anchors {
//            top: parent.top
//            left: parent.left
//            bottom: parent.bottom
//            topMargin: 40
//            leftMargin: 150
//        }

//        width: 1
//        color: "#321123"
//    }

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
        document: Collection.getDocument("/data/documents/note")
    }

    Column {
        id: toolBox
        width: 75
        height: 100
        spacing: 5
        visible: !rootItem.focusMode
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 5
            topMargin: 100
        }

        Rectangle {
            width: parent.width
            height: width
            radius: iconMargin * 2

            color: "white"

            border.width: 2

            Image {
                anchors.fill: parent
                anchors.margins: 5
                source: "qrc:/icons/clear page.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.clear()
                }
            }
        }

        Rectangle {
            width: parent.width
            height: width
            color: "white"
            border.width: 2
            radius: 10

            Image {
                anchors.fill: parent
                anchors.margins: iconMargin
                source: "qrc:/icons/undo.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.undo()
                }
            }
        }

        Rectangle {
            width: parent.width
            height: width
            radius: iconMargin * 2

            color: "white"
            border.width: 2

            Image {
                anchors.fill: parent
                anchors.margins: iconMargin
                source: "qrc:/icons/focus+.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    rootItem.focusMode = true
                }
            }
        }
    }
}

