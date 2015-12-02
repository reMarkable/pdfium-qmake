import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: document

    property var pageModel: []
    property int page: 0

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (page < pageModel.count - 1) {
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

    Repeater {
        id: pageRepeater
        model: document.pageModel
        delegate: Image {
            anchors.fill: document
            visible: document.page === index
            source: modelData
            smooth: false

            DrawingArea {
                id: drawingArea
                currentBrush: DrawingArea.Pen
                anchors.fill: parent

                Rectangle {
                    width: 100
                    height: width
                    border.width: 1
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "CLEAN"
                        horizontalAlignment: Text.AlignHCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            drawingArea.clear()
                        }
                    }
                }

                Rectangle {
                    width: 100
                    height: width
                    border.width: 1
                    anchors {
                        right: parent.right
                        top: parent.top
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "UNDO"
                        horizontalAlignment: Text.AlignHCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            drawingArea.undo()
                        }
                    }
                }
            }
        }
    }

    onPageModelChanged: {
        console.log(pageModel)
    }

    Rectangle {
        id: nextPageButton
        width: 100
        height: width
        visible: document.page < document.pageModel.count - 1
        border.width: 1
        anchors {
            bottom: parent.bottom
            right: parent.right
        }
        Text {
            anchors.centerIn: parent
            text: "NEXT\nPAGE"
            horizontalAlignment: Text.AlignHCenter
        }
        MouseArea {
            enabled: nextPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.page++
            }
        }
    }

    Rectangle {
        id: indexButton
        width: 100
        height: width
        border.width: 1
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        Text {
            anchors.centerIn: parent
            text: "INDEX"
            horizontalAlignment: Text.AlignHCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                thumbnailGrid.visible = !thumbnailGrid.visible
            }
        }
    }

    Rectangle {
        id: prevPageButton
        width: 100
        height: width
        visible: document.page > 0
        border.width: 1
        anchors {
            bottom: parent.bottom
            left: parent.left
        }
        Text {
            anchors.centerIn: parent
            text: "PREVIOUS\nPAGE"
            horizontalAlignment: Text.AlignHCenter
        }
        MouseArea {
            enabled: prevPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.page--
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: thumbnailGrid.visible
        color: "#7f7f7f7f"

        MouseArea {
            anchors.fill: parent
            enabled: thumbnailGrid.visible
            onClicked: thumbnailGrid.visible = false
        }
    }

    GridView {
        id: thumbnailGrid
        anchors.centerIn: parent
        width: 650
        height: 1200
        visible: false
        model: document.pageModel
        cellWidth: 300
        cellHeight: 400
        interactive: false
        delegate: Rectangle {
            width: 300
            height: 400
            border.width: document.page === index ? 5 : 1
            color: "white"

            Image {
                anchors.centerIn: parent
                height: parent.height - 20
                width: parent.width - 20
                source: modelData
                smooth: false
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
