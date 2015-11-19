import QtQuick 2.3
import QtQuick.Window 2.2

Window {
    id: window
    visible: true
    height: 1200
    width: 1600
    flags: Qt.Dialog

//    property string inactiveColor: "#999"
//    property string fontColor: "gray"
//    color: "#ddd"

    property string inactiveColor: "black"
    property string fontColor: "black"
    property string inactiveFontColor: "white"

    property int index: 0

    Item {
        id: rootItem
        anchors.centerIn: parent
        width: parent.height
        height: parent.width
        rotation: 90

        Keys.onPressed: {
            if (event.key === Qt.Key_Home) {
                window.index = 0
                event.accepted = true
                return
            }

            console.log("Key pressed: " + event.key)
        }

        Rectangle {
            id: topBar
            height: 40
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            color: "black"

            Row {
                anchors.fill: parent
                spacing: 1

                Rectangle {
                    id: homebutton
                    width: 100
                    height: parent.height
                    color: "white"
                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: "black"
                        text: "XO"
                        font.family: "Helvetica"
                    }
                    Rectangle {
                        anchors {
                            bottom: parent.bottom
                            left: parent.left
                            right: parent.right
                        }
                        height: 10
                        color: window.index === 0 ? "white" : "black"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            window.index = 0
                        }
                    }
                }

                Repeater {
                    id: tabRepeater
                    model: ["DOCUMENT", "SKETCH", "DESTIN"]
                    Rectangle {
                        width: 300
                        height: parent.height
                        Text {
                            id: text
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            color: "black"
                            text: modelData
                        }
                        Rectangle {
                            anchors {
                                bottom: parent.bottom
                                left: parent.left
                                right: parent.right
                            }
                            height: 10
                            color: window.index === index + 1 ? "white" : "black"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                window.index = index + 1
                            }
                        }
                    }
                }
            }
        }

        MainScreen {
            id: mainScreen
            visible: (window.index === 0)
            anchors {
                top: topBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            onNewNoteClicked: {
                window.index = 1
            }
            onArchiveClicked: {
                if (archiveIndex < 3) {
                    window.index = archiveIndex + 1
                }
            }
        }


        NoteArea {
            id: noteArea
            visible: window.index === 1
            anchors {
                top: topBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }
        }


        SketchArea {
            id: sketchArea
            visible: window.index === 2
            anchors {
                top: topBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }
        }

        Image {
            id: document
            visible: window.index === 3
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: topBar.bottom
            }

            onVisibleChanged: {
                if (visible) {
                    forceActiveFocus()
                } else {
                    rootItem.forceActiveFocus()
                }
            }

            Keys.onRightPressed: {
                if (file < 5) {
                    file++
                    event.accepted = true
                }
            }

            Keys.onLeftPressed: {
                if (file > 1) {
                    file--
                    event.accepted = true
                }
            }

            property int file: 1

            //source: "file:///home/sandsmark/Downloads/pdf-png/lol-" + file + ".png"
            source: "file:///data/pdf/" + file + ".png"

            Rectangle {
                id: nextPageButton
                width: 100
                height: width
                visible: document.file < 5
                border.width: 1
                anchors {
                    bottom: top.bottom
                    right: document.right
                }
                Text {
                    anchors.centerIn: parent
                    text: ">"
                }
                MouseArea {
                    enabled: nextPageButton.visible
                    anchors.fill: parent
                    onClicked: {
                        document.file++
                    }
                }
            }

            Rectangle {
                id: prevPageButton
                width: 100
                height: width
                visible: document.file > 1
                border.width: 1
                anchors {
                    bottom: top.bottom
                    left: document.left
                }
                Text {
                    anchors.centerIn: parent
                    text: "<"
                }
                MouseArea {
                    enabled: prevPageButton.visible
                    anchors.fill: parent
                    onClicked: {
                        document.file--
                    }
                }
            }
        }
    }
}
