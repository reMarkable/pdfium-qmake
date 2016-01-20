import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    property int iconMargin: 8

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Paintbrush
    }

    Column {
        id: toolBox
        width: 64
        height: 100
        visible: !rootItem.focusMode
        spacing: 5
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: iconMargin
            topMargin: 100
        }

        Rectangle {
            id: thickBrushSelect
            width: parent.width
            height: (width - iconMargin) * 4 + iconMargin
            border.width: drawingArea.currentBrush === DrawingArea.Paintbrush ? 4 : 2
            radius: 10
            color: drawingArea.currentBrush === DrawingArea.Paintbrush ? "gray" : "white"

            Image {
                id: brushIcon
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: toolBox.width - iconMargin * 2
                height: width
                anchors.margins: iconMargin
                source: "qrc:/icons/brush.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.currentBrush = DrawingArea.Paintbrush
                }
            }

            Rectangle {
                id: blackIcon
                anchors {
                    top: brushIcon.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 2
                }
                width: brushIcon.width
                height: width
                radius: width / 2

                border.width: drawingArea.currentColor === DrawingArea.Black ? 4 : 2
                border.color: "black"

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width / 2
                    height: width
                    radius: width / 2
                    color: "black"
                    border.width: 2
                    border.color: "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.Black
                    }
                }
            }


            Rectangle {
                id: grayIcon
                anchors {
                    top: blackIcon.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: iconMargin
                }
                width: brushIcon.width
                height: width
                radius: width / 2

                border.width: drawingArea.currentColor === DrawingArea.Gray ? 4 : 2
                border.color: "black"

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width / 2
                    height: width
                    radius: width / 2
                    color: "gray"
                    border.width: 2
                    border.color: "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.Gray
                    }
                }
            }


            Rectangle {
                id: whiteIcon
                anchors {
                    top: grayIcon.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: iconMargin
                }
                width: brushIcon.width
                height: width
                radius: width / 2

                border.width: drawingArea.currentColor === DrawingArea.White ? 4 : 2

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width / 2
                    height: width
                    radius: width / 2
                    color: "white"
                    border.width: 2
                    border.color: "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.White
                    }
                }
            }
        }

        Rectangle {
            id: thinBrushSelect
            width: parent.width
            height: width
            radius: iconMargin * 2

            border.width: drawingArea.currentBrush === DrawingArea.Pencil ? 3 : 2
            color: drawingArea.currentBrush === DrawingArea.Pencil ? "gray" : "white"

            Image {
                anchors.fill: parent
                anchors.margins: iconMargin
                source: "qrc:/icons/pencil.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.currentBrush = DrawingArea.Pencil
                }
            }
        }

        Rectangle {
            id: zoomSelect
            width: parent.width
            height: width
            radius: iconMargin * 2

            border.width: drawingArea.zoomtoolSelected ? 3 : 2
            color: drawingArea.zoomtoolSelected ? "gray" : "white"

            Image {
                anchors.fill: parent
                anchors.margins: iconMargin
                source: drawingArea.zoomFactor > 1 ? "qrc:/icons/xoom-.svg" : "qrc:/icons/xoom+.svg"
            }


            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (drawingArea.zoomFactor > 1 ) {
                        drawingArea.setZoom(0, 0, 1, 1)
                    } else {
                        drawingArea.zoomtoolSelected = !drawingArea.zoomtoolSelected
                    }
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
                source: "qrc:/icons/redo.svg"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    drawingArea.redo()
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

