import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: noteTab
    property int iconMargin: 8

    property string title: ""
    property int currentPage: 0
    property int pageCount: 0
    property string documentPath

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            return;
        }

        documentPath = document.path()
        title = Collection.title(documentPath)
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

    Component.onCompleted: forceActiveFocus()

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            if (document) {
                document.clearCache()
            }
        }
    }

    Keys.onRightPressed: {
        if (thumbnailGrid.visible) {
            if (thumbnailGrid.currentThumbnailPage < thumbnailGrid.thumbnailPageCount - 1) {
                thumbnailGrid.currentThumbnailPage++
            }
            return
        }

        moveForward()
        event.accepted = true
    }

    Keys.onLeftPressed: {
        if (thumbnailGrid.visible) {
            if (thumbnailGrid.currentThumbnailPage > 0) {
                thumbnailGrid.currentThumbnailPage--
            }
            return
        }

        moveBackward()
        event.accepted = true
    }

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
            height: 100
            border.width: 4
            radius: 5

            Rectangle {
                width: parent.width
                height: drawingArea.smoothFactor * parent.height / 1000
                color: "gray"
                border.width: 4
                radius: 5
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


    DocumentPositionBar {
        id: positionBar

        pageCount: noteTab.pageCount
        currentPage: noteTab.currentPage
        document: noteTab.document
    }

    ThumbnailGrid {
        id: thumbnailGrid
        currentThumbnailPage: noteTab.currentPage / maxDisplayItems
        thumbnailPageCount: Math.ceil(noteTab.pageCount / maxDisplayItems)
        documentPath: noteTab.documentPath
        pageCount: noteTab.pageCount
    }
}
