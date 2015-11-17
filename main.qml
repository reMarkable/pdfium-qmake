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
        anchors.centerIn: parent
        width: parent.height
        height: parent.width
        rotation: 90

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
                    model: ["DOCUMENT NAME", "SKETCH NAME"]
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
        }


        SketchArea {
            id: sketchArea
            visible: window.index === 1
            anchors {
                top: topBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            Text {
                anchors.centerIn: parent
                font.family: "Comic Sans MS"
                font.pointSize: 40
                text: "HEI LOL"
            }
        }


        SketchArea {
            id: sketchArea2
            visible: window.index === 2
            anchors {
                top: topBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            Text {
                x: 300
                y: 400
                font.family: "Comic Sans MS"
                font.pointSize: 30
                text: "kekekekeke"
            }
        }
    }
}
