import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: sketchTab
    property int iconMargin: 8

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            return;
        }
    }

    Component.onDestruction: {
        if (document) {
            document.destroy()
        }
    }

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Paintbrush
        document: sketchTab.document
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

    Column {
        width: 64
        visible: !rootItem.focusMode
        spacing: 5
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: iconMargin
            bottomMargin: 20
        }

        ToolButton {
            icon: "qrc:/icons/Charge.svg"
            active: drawingArea.predictionEnabled
            onClicked: {
                drawingArea.doublePredictionEnabled = false
                drawingArea.predictionEnabled = ! drawingArea.predictionEnabled
            }
        }


        ToolButton {
            icon: "qrc:/icons/Charge.svg"
            active: drawingArea.doublePredictionEnabled
            onClicked: {
                drawingArea.predictionEnabled = false
                drawingArea.doublePredictionEnabled = ! drawingArea.doublePredictionEnabled
            }

            Image {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 8
                width: 24
                height: width
                source: "qrc:/icons/pluss.svg"
            }
        }

        Rectangle {
            width: 64
            height: 400
            border.width: 4
            radius: 5

            Rectangle {
                width: parent.width
                height: drawingArea.smoothFactor * parent.height / 1000
                color: "gray"
                border.width: 4
                radius: 5
            }

            Text {
                anchors.centerIn: parent
                rotation: 90
                text: "smoothing"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var smoothFactor = 1000 * mouse.y / height
                    if (smoothFactor < 300) smoothFactor = 0
                    drawingArea.smoothFactor = smoothFactor
                }
            }
        }
    }
}

