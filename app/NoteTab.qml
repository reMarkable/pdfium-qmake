import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    Column {
        anchors.fill: parent
        anchors.topMargin: 75
        anchors.leftMargin: 20
        spacing: 50
        Repeater {
            model: 32
            Rectangle {
                width: parent.width - 30
                height: 1
                color: "#321123"
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
            topMargin: 40
            leftMargin: 150
        }

        width: 1
        color: "#321123"
    }

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
    }

    Column {
        id: toolBox
        width: 75
        height: 100
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 5
            topMargin: 100
        }
        Rectangle {
            width: parent.width
            height: width
            color: "white"
            border.width: 1

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "CLEAR"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.clear()
                    }
                }
            }
        }
    }
}

