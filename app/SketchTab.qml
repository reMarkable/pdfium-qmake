import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    property int iconMargin: 8

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Paintbrush
        document: Collection.getDocument("/data/documents/sketch")
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

        ToolButton {
            id: brushButton
            icon: "qrc:/icons/brush.svg"
            active: drawingArea.currentBrush === DrawingArea.Paintbrush
            onClicked: drawingArea.currentBrush = DrawingArea.Paintbrush

            height: (width - iconMargin) * 4 + iconMargin

            Column {
                spacing: iconMargin
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    margins: iconMargin
                }

                ColorButton {
                    id: blackIcon
                    color: "black"
                    highlightColor: "white"
                    active: brushButton.active && drawingArea.currentColor === DrawingArea.Black
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.Black
                    }
                }

                ColorButton {
                    id: grayIcon
                    color: "gray"
                    highlightColor: "white"
                    active: brushButton.active && drawingArea.currentColor === DrawingArea.Gray
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.Gray
                    }
                }


                ColorButton {
                    id: whiteIcon
                    color: "white"
                    highlightColor: "black"
                    active: brushButton.active && drawingArea.currentColor === DrawingArea.White
                    onClicked: {
                        drawingArea.currentBrush = DrawingArea.Paintbrush
                        drawingArea.currentColor = DrawingArea.White
                    }
                }
            }
        }

        ToolButton {
            icon: "qrc:/icons/pencil.svg"
            active: drawingArea.currentBrush === DrawingArea.Pencil
            onClicked: {
                drawingArea.currentBrush = DrawingArea.Pencil
            }
        }

        ToolButton {
            icon: "qrc:/icons/eraser.svg"
            active: drawingArea.currentBrush === DrawingArea.Eraser
            onClicked: {
                drawingArea.currentBrush = DrawingArea.Eraser
            }
        }

        ToolButton {
            icon: drawingArea.zoomFactor > 1 ? "qrc:/icons/xoom-.svg" : "qrc:/icons/xoom+.svg"
            active: drawingArea.zoomtoolSelected
            onClicked: {
                if (drawingArea.zoomFactor > 1 ) {
                    drawingArea.setZoom(0, 0, 1, 1)
                } else {
                    drawingArea.zoomtoolSelected = !drawingArea.zoomtoolSelected
                }
            }
        }

        ToolButton {
            icon: "qrc:/icons/clear page.svg"
            onClicked: drawingArea.clear()
        }

        ToolButton {
            icon: "qrc:/icons/undo.svg"
            onClicked: drawingArea.undo()
        }

        ToolButton {
            icon: "qrc:/icons/redo.svg"
            onClicked: drawingArea.redo()
        }

        ToolButton {
            icon: "qrc:/icons/focus+.svg"
            onClicked: rootItem.focusMode = true
        }
    }
}

