import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: document

    property string documentPath
    onDocumentPathChanged: pageModel = Collection.getPages(documentPath)
    property var pageModel: []
    property int page: 0

    Component.onDestruction: {
        for (var i=0; i<pageModel.length; i++) {
            pageModel[i].destroy()
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
        if (page < pageModel.length - 1) {
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
            page: document.pageModel.length === 0 ? null : document.pageModel[document.page]
        }

        Column {
            id: toolBox
            width: 75
            height: 100
            visible: !rootItem.focusMode && !thumbnailGrid.visible
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 5
                topMargin: 100
            }
            Rectangle {
                width: parent.width
                height: width
                color: "white"
                border.width: 1

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "CLEAR"
                    font.pointSize: 7
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            drawingArea.clear()
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: width
                color: "white"
                border.width: 1

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "UNDO"
                    font.pointSize: 7
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            drawingArea.undo()
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: width
                color: "white"
                border.width: 1

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "INDEX"
                    font.pointSize: 7
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            thumbnailGrid.visible  = true
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: width
                color: "white"
                border.width: 1

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "FOCUS"
                    font.pointSize: 7
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            rootItem.focusMode = true
                        }
                    }
                }
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
