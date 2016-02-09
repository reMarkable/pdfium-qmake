import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: document
    property int iconMargin: 8

    property string documentPath
    onDocumentPathChanged: drawingArea.document = Collection.getDocument(documentPath)
    property int page: 0
    onPageChanged: {
        if (drawingArea.document) {
            drawingArea.document.currentIndex = page
        }
    }

    Component.onDestruction: {
        drawingArea.document.destroy()
    }

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (page < drawingArea.document.pageCount - 1) {
            page++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
        if (page > 0) {
            page--
            event.accepted = true
        }
    }

    Rectangle {
        anchors.fill: parent

        DrawingArea {
            id: drawingArea
            currentBrush: DrawingArea.Pen
            anchors.fill: parent
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
        }
    }


    Item {
        anchors.fill: parent
        Rectangle {
            id: thumbnailGrid
            anchors.fill: parent
            visible: false
            color: "#7f7f7f7f"

            MouseArea {
                anchors.fill: parent
                enabled: thumbnailGrid.visible
                onClicked: thumbnailGrid.visible = false
            }

            GridView {
                anchors.fill: parent
                anchors.margins: 10
                model: document.pageModel
                cellWidth: 290
                cellHeight: 390
                interactive: false


                delegate: Rectangle {
                    width: 290
                    height: 390
                    border.width: document.page === index ? 5 : 1
                    color: "white"

                    Image {
                        anchors.centerIn: parent
                        height: parent.height - 20
                        width: parent.width - 20
                        source: "file://" + modelData.backgroundPath()
                        smooth: false
                        asynchronous: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            document.page = index
                            thumbnailGrid.visible = false
                        }
                    }
                }
            }
        }
    }
}
