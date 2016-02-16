import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: noteTab
    property int iconMargin: 8

    property int currentPage: 0
    property int pageCount: 0

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            return;
        }

        templateRepeater.model = document.availableTemplates()
        pageCount = Qt.binding(function() { return document.pageCount; })
        currentPage = Qt.binding(function() { return document.currentIndex; })
    }

    function moveForward() {
        if (document.currentIndex < pageCount - 1) {
            document.currentIndex++
        }
    }

    function moveBackward() {
        if (document.currentIndex > 0) {
            document.currentIndex--
        }
    }

    Component.onDestruction: {
        if (document) {
            document.destroy()
        }
    }

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        moveForward()
        event.accepted = true
    }

    Keys.onLeftPressed: {
        moveBackward()
        event.accepted = true
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

            Image {
                anchors.centerIn: parent
                width: 24
                height: width
                source: "qrc:/icons/no.svg"
            }
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
            icon: "qrc:/icons/page-type.png"
            onClicked: {
                templateSelectRow.visible = !templateSelectRow.visible
            }


            Image {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 8
                width: 24
                height: width
                source: "qrc:/icons/forward.svg"
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
                noteTab.document.addPage()
                //console.log(noteTab.document.availableTemplates())
            }
        }

        ToolButton {
            icon: "qrc:/icons/forward.svg"
            onClicked: {
                noteTab.moveForward()
            }
        }

        ToolButton {
            icon: "qrc:/icons/back.svg"
            onClicked: {
                noteTab.moveBackward()
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
            height: 500
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


    Item {
        id: positionBar
        anchors {
            bottom: parent.bottom
            right: parent.right
            rightMargin: 100
            left: parent.left
            leftMargin: 100
        }


        height: 7

        Rectangle {
            anchors.fill: parent
            color: "#999"
            radius: 2

            visible: noteTab.pageCount >= 100

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: noteTab.pageCount > 0 ? parent.width * (noteTab.currentPage / noteTab.pageCount) : 0
                color: "#333"
                radius: 2
            }
        }

        Row {
            id: positionRow
            anchors.centerIn: parent
            spacing: 2

            visible: noteTab.pageCount < 100

            Repeater {
                id: positionRepeater
                model: visible ? noteTab.pageCount : 0
                delegate: Rectangle {
                    height: positionBar.height
                    width: Math.min(positionBar.width / positionRepeater.count - positionRow.spacing, 50)
                    color: noteTab.currentPage < index ? "#999" : "#333"
                    radius: 2
                }
            }
        }
    }
}
