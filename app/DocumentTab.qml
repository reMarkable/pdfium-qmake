import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: documentTab

    property string title: ""

    property int iconMargin: 8

    property int pageCount: 0
    property int currentPage: 0

    property QtObject document
    property string documentPath
    onDocumentPathChanged: {
        title = Collection.title(documentPath)
        document = Collection.getDocument(documentPath)
        pageCount = Qt.binding(function() { return document.pageCount; })
        currentPage = Qt.binding(function() { return document.currentIndex; })
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

        if (document.currentIndex < pageCount - 1) {
            document.currentIndex++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
        if (thumbnailGrid.visible) {
            if (thumbnailGrid.currentThumbnailPage > 0) {
                thumbnailGrid.currentThumbnailPage--
            }
            return
        }

        if (document.currentIndex > 0) {
            document.currentIndex--
            event.accepted = true
        }
    }

    Rectangle {
        anchors.fill: parent

        DrawingArea {
            id: drawingArea
            currentBrush: DrawingArea.Pen
            anchors.fill: parent
            document: documentTab.document
        }


        ToolBox {
            id: toolBox
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: iconMargin
                topMargin: 100
            }

            document: documentTab.document
            drawingArea: drawingArea
        }
    }

    ThumbnailGrid {
        id: thumbnailGrid
        currentThumbnailPage: documentTab.currentPage / maxDisplayItems
        thumbnailPageCount: Math.ceil(documentTab.pageCount / maxDisplayItems)
        documentPath: documentTab.documentPath
        pageCount: documentTab.pageCount
    }

    DocumentPositionBar {
        id: positionBar

        pageCount: documentTab.pageCount
        currentPage: documentTab.currentPage
        document: documentTab.document
    }
}
