import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: documentTab
    property int iconMargin: 8

    property int pageCount: 0
    property int currentPage: 0

    property QtObject document
    property string documentPath
    onDocumentPathChanged: {
        document = Collection.getDocument(documentPath)
        pageCount = Qt.binding(function() { return document.pageCount; })
        currentPage = Qt.binding(function() { return document.currentIndex; })
    }

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (document.currentIndex < pageCount - 1) {
            document.currentIndex++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
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
                icon: "qrc:/icons/Index.svg"
                active: thumbnailGrid.visible
                onClicked: thumbnailGrid.visible = true
            }
        }
    }

    ThumbnailGrid {
        id: thumbnailGrid
        currentThumbnailPage: documentTab.currentPage / maxDisplayItems
        thumbnailPageCount: Math.ceil(documentTab.pageCount / maxDisplayItems)
        documentPath: documentTab.documentPath
        pageCount: documentTab.pageCount
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

            visible: documentTab.pageCount >= 100

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: documentTab.pageCount > 0 ? parent.width * (documentTab.currentPage / documentTab.pageCount) : 0
                color: "#333"
                radius: 2
            }
        }

        Row {
            id: positionRow
            anchors.centerIn: parent
            spacing: 2

            visible: documentTab.pageCount < 100

            Repeater {
                id: positionRepeater
                model: visible ? documentTab.pageCount : 0
                delegate: Rectangle {
                    height: positionBar.height
                    width: Math.min(positionBar.width / positionRepeater.count - positionRow.spacing, 50)
                    color: documentTab.currentPage < index ? "#999" : "#333"
                    radius: 2
                }
            }
        }
    }
}
