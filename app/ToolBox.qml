import QtQuick 2.0
import com.magmacompany 1.0

Column {
    id: toolBox

    visible: !rootItem.focusMode

    property var availableButtons: {
        "Brush": brushComponent,
        "Pencil": pencilComponent,
        "Pen": penComponent,
        "Eraser": eraserComponent,
        "Zoom": zoomComponent,
        "Clear": clearComponent,
        "Undo": undoComponent,
        "Redo": redoComponent,
        "Focus": focusComponent,
        "TemplateSelect": templateSelectComponent,
        "NewPage": newPageComponent,
        "Index": indexComponent
    }

    property QtObject document

    property string currentTemplate: document.currentTemplate

    onCurrentTemplateChanged: {
        if (!document) {
            return
        }
        if (currentTemplate === "Sketch") {
            buttonRepeater.model = [ "Brush", "Pencil", "Eraser", "Clear", "Undo", "Redo", "Focus","Index", "TemplateSelect", "NewPage" ]
        } else if (currentTemplate === "Document") {
            buttonRepeater.model = [ "Pen", "Clear", "Undo", "Redo", "Focus", "Index" ]
        } else {
            buttonRepeater.model = [ "Pen", "Clear", "Undo", "Redo", "Focus", "Index", "TemplateSelect", "NewPage" ]
        }
    }

    Repeater {
        id: buttonRepeater

        delegate: Loader {
            sourceComponent: toolBox.availableButtons[modelData]
        }
    }
    
    Component {
        id: brushComponent

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
    }

    Component {
        id: pencilComponent
        ToolButton {
            icon: "qrc:/icons/pencil.svg"
            active: drawingArea.currentBrush === DrawingArea.Pencil
            onClicked: {
                drawingArea.currentBrush = DrawingArea.Pencil
            }
        }
    }

    Component {
        id: penComponent
        ToolButton {
            icon: "qrc:/icons/pen.svg"
            active: drawingArea.currentBrush === DrawingArea.Pen
            onClicked: {
                drawingArea.currentBrush = DrawingArea.Pen
            }
        }
    }
    
    Component {
        id: eraserComponent

        ToolButton {
            icon: "qrc:/icons/eraser.svg"
            active: drawingArea.currentBrush === DrawingArea.Eraser
            onClicked: {
                drawingArea.currentBrush = DrawingArea.Eraser
            }
        }
    }
    
    Component {
        id: zoomComponent

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
    }

    Component {
        id: clearComponent

        ToolButton {
            icon: "qrc:/icons/clear page.svg"
            onClicked: drawingArea.clear()
        }
    }

    Component {
        id: undoComponent

        ToolButton {
            icon: "qrc:/icons/undo.svg"
            onClicked: drawingArea.undo()
        }
    }
    
    Component {
        id: redoComponent

        ToolButton {
            icon: "qrc:/icons/redo.svg"
            onClicked: drawingArea.redo()
        }
    }
    
    Component {
        id: focusComponent

        ToolButton {
            icon: "qrc:/icons/focus+.svg"
            onClicked: rootItem.focusMode = true
        }
    }

    Component {
        id: templateSelectComponent


        ToolButton {
            id: templateSelectIcon
            icon: "qrc:/icons/Template.svg"
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
                    model: toolBox.document !== null ? toolBox.document.availableTemplates() : 0
                    delegate: Rectangle {
                        height: templateSelectIcon.height
                        width: height * 2
                        border.width: 2
                        color: toolBox.document !== null ? (toolBox.document.currentTemplate === modelData ? "gray" : "white") : "white"
                        Text {
                            anchors.centerIn: parent
                            text: modelData
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                templateSelectRow.visible = false
                                toolBox.document.currentTemplate = modelData
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: newPageComponent

        ToolButton {
            icon: "qrc:/icons/new page.svg"
            onClicked: {
                toolBox.document.addPage()
            }
        }
    }

    Component {
        id: forwardComponent
        ToolButton {
            icon: "qrc:/icons/forward.svg"
            onClicked: {
                if (toolBox.document.currentPage < toolBox.document.pageCount - 1) {
                    document.currentPage++
                }
            }
        }
    }
    Component {
        id: backComponent

        ToolButton {
            icon: "qrc:/icons/back.svg"
            onClicked: {
                if (toolBox.document.currentPage > 0) {
                    toolBox.document.currentPage--
                }
            }
        }
    }

    Component {
        id: indexComponent

        ToolButton {
            icon: "qrc:/icons/Index.svg"
            active: thumbnailGrid.visible
            onClicked: thumbnailGrid.visible = true
        }
    }
}
