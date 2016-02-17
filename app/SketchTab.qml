import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: sketchTab
    property int iconMargin: 8

    property string documentPath
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

    ToolBox {
        id: toolBox
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: iconMargin
            topMargin: 100
        }

        buttons: ["Brush", "Pencil", "Eraser", "Clear", "Undo", "Redo", "Focus"]
        document: sketchTab.document
        drawingArea: drawingArea
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

