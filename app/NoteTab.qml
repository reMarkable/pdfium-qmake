import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: noteTab
    property int iconMargin: 8

    property string title: ""
    property string tabIdentifier: "note"

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            return;
        }
        var path = document.path
        tabIdentifier = path
        title = Qt.binding(function() { return document.title; })
    }

    function moveForward() {
        if (document.currentPage < document.pageCount - 1) {
            document.currentPage++
        }
    }

    function moveBackward() {
        if (document.currentPage > 0) {
            document.currentPage--
        }
    }

    Component.onCompleted: forceActiveFocus()

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
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

    Keys.forwardTo: rootItem

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
                sourceSize.width: width
                sourceSize.height: height
                source: "qrc:/icons/pluss.svg"
                asynchronous: true
            }
        }

        Rectangle {
            width: 64
            height: 100
            radius: 5
            border.width: 4
            border.color: "gray"

            Rectangle {
                width: parent.width
                height: drawingArea.smoothFactor * parent.height / 1000
                color: "darkGray"
                border.width: 4
                border.color: "gray"
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
        anchors {
            bottom: parent.bottom
            right: parent.right
            rightMargin: 100
            left: parent.left
            leftMargin: 100
        }

        pageCount: noteTab.document.pageCount
        currentPage: noteTab.document.currentPage
        onPageClicked: noteTab.document.currentPage = page
    }

    Rectangle {
        id: thumbnailGrid
        width: rootItem.width
        height: rootItem.height
        x: -viewRoot.x
        y: -viewRoot.y
        visible: false

        onVisibleChanged: {
            if (visible) {
                drawingArea.storeThumbnail()
            }
        }

        color: "#7f000000"

        MouseArea {
            anchors.fill: parent
            enabled: thumbnailGrid.visible
            onClicked: thumbnailGrid.visible = false
        }

        BookOverview {
            document: noteTab.document
            anchors {
                fill: parent
                topMargin: 75
            }
            onPageClicked:{
                thumbnailGrid.visible = false
                noteTab.document.currentPage = index
            }
            onVisibleChanged: {
                if (visible) {
                    forceActiveFocus()
                } else {
                    noteTab.forceActiveFocus()
                }
            }
        }
    }
}
