import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: noteTab
    property int iconMargin: 8

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            return;
        }

        templateRepeater.model = document.availableTemplates()
    }

    Component.onDestruction: {
        if (document) {
            document.destroy()
        }
    }

    property string documentPath
    onDocumentPathChanged: {
        if (documentPath === "") {
            return;
        }

        document = Collection.getDocument(documentPath)
    }

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
        document: noteTab.document
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
            id: templateSelectIcon
            icon: "qrc:/icons/forward.svg"
            onClicked: {
                templateSelectRow.visible = !templateSelectRow.visible
            }


            Row {
                id: templateSelectRow
                visible: false
                anchors {
                    left: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }

                Repeater {
                    id: templateRepeater
                    delegate: Rectangle {
                        height: templateSelectIcon.height
                        width: height * 2
                        border.width: 2
                        color: noteTab.document !== null ? (noteTab.document.currentTemplate === modelData ? "gray" : "white") : "white"
                        Text {
                            anchors.centerIn: parent
                            text: modelData
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                templateSelectRow.visible = false
                                noteTab.document.currentTemplate = modelData
                            }
                        }
                    }
                }
            }
        }

        ToolButton {
            icon: "qrc:/icons/new page.svg"
            onClicked: {
                console.log(noteTab.document.availableTemplates())
            }
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
