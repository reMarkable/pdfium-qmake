import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: document

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (page < 4) {
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

    property int page: 0

    Repeater {
        id: pageRepeater
        model: 5
        delegate: Image {
            anchors.fill: document
            visible: document.page === modelData
            source: "file:///data/pdf/" + (index + 1) + ".png"


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

    Rectangle {
        id: nextPageButton
        width: 100
        height: width
        visible: document.page < 4
        border.width: 1
        anchors {
            bottom: parent.bottom
            right: document.right
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
            left: document.left
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

    GridView {
        id: thumbnailGrid
        anchors.centerIn: parent
        width: 650
        height: 1200
        visible: false
        model: 5
        cellWidth: 300
        cellHeight: 400
        interactive: false
        delegate: Rectangle {
            width: 300
            height: 400
            border.width: document.page === index ? 2 : 1
            color: "white"
            Image {
                anchors.centerIn: parent
                height: parent.height - 5
                width: parent.width - 5
                source: "file:///data/pdf/" + (index + 1) + ".png"
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
