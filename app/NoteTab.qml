import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    property int iconMargin: 8

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
        document: Collection.getDocument("/data/documents/note")
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
            height: 800
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
                    if (smoothFactor < 50) smoothFactor = 0
                    drawingArea.smoothFactor = smoothFactor
                }
            }
        }
    }
}

