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
        anchors.fill: parent

        delegate: Rectangle {
            anchors.fill: pageRepeater
            visible: document.page === index
            Image {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }

                source: modelData
                smooth: false
                asynchronous: true
            }

            DrawingArea {
                id: drawingArea
                currentBrush: DrawingArea.Pen
                anchors.fill: parent
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
    }

    onPageModelChanged: {
        console.log(pageModel)
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
                        source: modelData
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
