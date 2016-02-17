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

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
        document: noteTab.document
    }


    ToolBox {
        id: toolBox
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: iconMargin
            topMargin: 100
        }

        document: noteTab.document
        drawingArea: drawingArea

        buttons: [
            "Pen",
            "Clear",
            "Undo",
            "Redo",
            "Focus",
            "TemplateSelect",
            "NewPage",
            "Forward",
            "Back",
            "Index"
        ]
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

    ThumbnailGrid {
        id: thumbnailGrid
        currentThumbnailPage: noteTab.currentPage / maxDisplayItems
        thumbnailPageCount: Math.ceil(noteTab.pageCount / maxDisplayItems)
        documentPath: noteTab.documentPath
        pageCount: noteTab.pageCount
    }
}
