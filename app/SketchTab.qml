import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Paintbrush
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

            border.color: penToolList.visible ? "black" : "white"

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "PEN\nTOOL"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        penToolList.visible = !penToolList.visible
                    }
                }
            }
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

        Rectangle {
            width: parent.width
            height: width
            color: "white"
            border.width: 1

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "UNDO"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.undo()
                    }
                }
            }
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
                text: "REDO"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.redo()
                    }
                }
            }
        }
    }

    Row {
        id: penToolList
        visible: false
        height: 75
        width: 300
        anchors {
            top: toolBox.top
            left: toolBox.right
        }

        Rectangle {
            id: thickBrushSelect
            height: parent.height
            width: height
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: 50
                height: width
                radius: width
                color: "black"
            }


            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.currentBrush = DrawingArea.Paintbrush
                    penToolList.visible = false
                }
            }
        }

        Rectangle {
            id: thinBrushSelect
            height: parent.height
            width: height
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: 20
                height: width
                radius: width
                color: "black"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.currentBrush = DrawingArea.Pencil
                    penToolList.visible = false
                }
            }
        }
    }
}

