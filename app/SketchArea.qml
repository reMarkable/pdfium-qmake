import QtQuick 2.0
import com.magmacompany 1.0

DrawingArea {
    Column {
        anchors.fill: parent
        anchors.topMargin: 75
        anchors.leftMargin: 20
        spacing: 30
        Repeater {
            model: 50
            Rectangle {
                width: parent.width - 30
                height: 1
                color: "#321123"
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
            topMargin: 40
            leftMargin: 150
        }

        width: 1
        color: "#321123"
    }

    Column {
        id: toolBox
        width: 75
        height: 100
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 5
            topMargin: 100
        }
        Rectangle {
            width: parent.width
            height: width

            border.color: penToolList.visible ? "black" : "white"

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "PEN\nTOOL"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        penToolList.visible = !penToolList.visible
                    }
                }
            }
        }
        Rectangle {
            id: regretToolSelect
            width: parent.width
            height: width

            border.color: clicked ? "black" : "white"

            property bool clicked: false

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "REGRET\nTOOL"
                font.pointSize: 7
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        regretToolSelect.clicked = !regretToolSelect.clicked
                        penToolList.visible = false
                    }
                }
            }
        }
    }

    Row {
        id: penToolList
        visible: false
        height: 75
        width: 300
        anchors {
            top: toolBox.top
            left: toolBox.right
        }

        Rectangle {
            id: thickBrushSelect
            height: parent.height
            width: height
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: 50
                height: width
                radius: width
                color: "black"
            }
        }

        Rectangle {
            id: thinBrushSelect
            height: parent.height
            width: height
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: 20
                height: width
                radius: width
                color: "black"
            }
        }

        Rectangle {
            id: colorBlackSelect
            height: parent.height
            width: height
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: parent.height - 20
                height: width
                color: "black"
                border {
                    width: 2
                    color: "white"
                }

                Text {
                    anchors.centerIn: parent
                    text: "BLACK"
                    color: "white"
                    font.pointSize: 7
                }
            }
        }
    }
}

