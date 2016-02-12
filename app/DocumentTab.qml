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
        drawingArea.document = Collection.getDocument(documentPath)
        pageCount = Qt.binding(function() { return drawingArea.document.pageCount; })
        currentPage = Qt.binding(function() { return drawingArea.document.currentIndex; })
    }

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (drawingArea.document.currentIndex < pageCount - 1) {
            drawingArea.document.currentIndex++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
        if (drawingArea.document.currentIndex > 0) {
            drawingArea.document.currentIndex--
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

            ToolButton {
                icon: "qrc:/icons/Index.svg"
                active: thumbnailGrid.visible
                onClicked: thumbnailGrid.visible = true
            }
        }
    }

    Rectangle {
        id: thumbnailGrid
        width: rootItem.width
        height: rootItem.height
        x: -viewRoot.x
        y: -viewRoot.y
        visible: false

        color: "#7f000000"

        property int maxDisplayItems: 9//(rootItem.rotation === 0) ? 8 : 9
        property int currentThumbnailPage: documentTab.currentPage / maxDisplayItems
        property int thumbnailPageCount: Math.ceil(documentTab.pageCount / maxDisplayItems)


        MouseArea {
            anchors.fill: parent
            enabled: thumbnailGrid.visible
            onClicked: thumbnailGrid.visible = false
        }

        Rectangle {
            anchors {
                top: parent.top
                right: modeSelect.left
                topMargin: 75
                rightMargin: 10
            }

            width: height
            height: 75
            color: "#a0000000"
            radius: 5

            Image {
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                    margins: 10
                }
                width: height
                source: "qrc:///icons/yes_white.svg"
            }
        }

        Rectangle {
            id: modeSelect
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 75
                rightMargin: 50
            }

            width: 150
            height: 75
            color: "#7f000000"
            radius: 5

            Image {
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                    margins: 5
                }
                width: height
                source: thumbnailGrid.maxDisplayItems === 9 ? "qrc:///icons/Grid 9.svg" : "qrc:///icons/Grid 9_white.svg"

                MouseArea {
                    anchors.fill: parent
                    onClicked: thumbnailGrid.maxDisplayItems = 9
                }
            }

            Image {
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    margins: 5
                }
                width: height
                source: thumbnailGrid.maxDisplayItems === 36 ? "qrc:///icons/Grid 36.svg" : "qrc:///icons/Grid 36_white.svg"

                MouseArea {
                    anchors.fill: parent
                    onClicked: thumbnailGrid.maxDisplayItems = 36
                }
            }
        }

        Grid {
            id: thumbGrid
            anchors {
                centerIn: parent
            }
            columns: (thumbnailGrid.maxDisplayItems === 9 ) ? 3 : 6
            rows: (thumbnailGrid.maxDisplayItems === 9 ) ? 3 : 6
//            columns: (rootItem.rotation === 0) ? 4 : 3
//            rows: (rootItem.rotation === 0) ? 2 : 3

            spacing: (thumbnailGrid.maxDisplayItems === 9) ? 50 : 10

            Repeater {
                model: thumbnailGrid.visible ? Math.min(thumbnailGrid.maxDisplayItems, documentTab.pageCount - thumbnailGrid.currentThumbnailPage * thumbnailGrid.maxDisplayItems) : 0

                delegate: Rectangle {
                    id: thumbnailDelegate
                    width: (thumbnailGrid.maxDisplayItems === 9 ) ? 270 : 150
                    height: (thumbnailGrid.maxDisplayItems === 9 ) ? 370 : 200
                    color: "white"
                    property int pageNumber: modelData + thumbnailGrid.currentThumbnailPage * thumbnailGrid.maxDisplayItems
                    border.color: "black"
                    border.width: documentTab.currentPage === pageNumber ? 8 : 2

                    Image {
                        anchors.fill: parent
                        anchors.margins: 8
                        source: "file://" + documentTab.documentPath + "-" + thumbnailDelegate.pageNumber + ".thumbnail.jpg"
                        smooth: false
                        asynchronous: true

                        Rectangle {
                            anchors {
                                top: parent.top
                                right: parent.right
                            }
                            width: 75
                            height: 75
                            color: "#a0000000"
                            radius: 5

                            Text {
                                anchors.centerIn: parent
                                color: "white"
                                text: (thumbnailDelegate.pageNumber + 1) + "/" + documentTab.pageCount
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log(thumbnailDelegate.pageNumber)
                            drawingArea.document.currentIndex = thumbnailDelegate.pageNumber
                            thumbnailGrid.visible = false
                        }
                    }
                }
            }
        }


        Item {
            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
                margins: 50
            }
            height: 75

            Row {
                id: pageRow
                anchors.centerIn: parent

                spacing: 10

                Repeater {
                    model: thumbnailGrid.thumbnailPageCount
                    delegate: Rectangle {
                        width: 30
                        height: width
                        radius: 2
                        color: thumbnailGrid.currentThumbnailPage === index ? "black" : "transparent"
                        border.width: 2
                        border.color: "white"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: thumbnailGrid.currentThumbnailPage = index
                        }
                    }
                }
            }


            Image {
                anchors.right: parent.right
                width: height
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                source: "qrc:///icons/forward_white.svg"
                visible: (thumbnailGrid.currentThumbnailPage < thumbnailGrid.thumbnailPageCount - 1)

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (thumbnailGrid.currentThumbnailPage < thumbnailGrid.thumbnailPageCount - 1) {
                            thumbnailGrid.currentThumbnailPage++
                        }
                    }
                }
            }

            Image {
                anchors.left: parent.left
                width: height
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                source: "qrc:///icons/back_white.svg"
                visible: (thumbnailGrid.currentThumbnailPage > 0)

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (thumbnailGrid.currentThumbnailPage > 0) {
                            thumbnailGrid.currentThumbnailPage--
                        }
                    }
                }
            }
        }
    }
}
