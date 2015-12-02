import QtQuick 2.0

Rectangle {
    id: tabBar
    color: "black"

    property var objectList: []
    property var tabModel: []
    property int currentTab: 0

    Row {
        anchors.fill: parent
        spacing: 1

        Rectangle {
            id: homebutton
            width: 100
            height: tabBar.height
            visible: tabBar.height > 0
            color: "white"
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "black"
                text: "HOME"
                font.family: "Helvetica"
            }
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                height: 10
                color: tabBar.currentTab === 0 ? "white" : "black"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    tabBar.currentTab = 0
                }
            }
        }

        Repeater {
            id: tabRepeater
            model: tabBar.tabModel
            Rectangle {
                width: 200
                visible: tabBar.height > 0
                height: tabBar.height
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
                    color: tabBar.currentTab === index + 1 ? "white" : "black"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        tabBar.currentTab = index + 1
                    }
                }

                Rectangle {
                    height: 30
                    width: 30
                    color: "black"
                    visible: index > 0
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        rightMargin: 15
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "X"
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tabBar.currentTab = 0

                            tabBar.objectList[index-1].destroy()
                            tabBar.objectList.splice(index-1, 1)

                            var tabModel = tabBar.tabModel
                            tabModel.splice(index, 1)
                            tabBar.tabModel = tabModel
                        }
                    }
                }
            }
        }
    }
}
