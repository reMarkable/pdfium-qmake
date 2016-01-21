import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: tabBar
    //color: "white"

    property var objectList: []
    property var tabModel: []
    property int currentTab: 0
    property int tabBorderSize: 4
    property int tabRadius: 10
    property int marginTop: 5

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: tabRadius + marginTop
        height: tabBorderSize
        color: "black"
    }

    Row {
        id: tabRow
        anchors.fill: parent
        anchors.topMargin: marginTop
        anchors.leftMargin: marginTop
        spacing: 1

        Rectangle {
            id: homebutton
            width: 75
            height: tabBar.height - marginTop
            visible: tabBar.height > 0
            border.width: tabBorderSize
            border.color: tabBar.currentTab === 0 ? "black" : "lightGray"
            radius: tabRadius
            //color: "black"
            color: tabBar.currentTab === 0 ? "white" : "lightGray"
            Image {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                width: 35
                height: 35
                smooth: true
                source: "qrc:/icons/home_small.svg"
            }

            Rectangle {
                anchors {
                    top: parent.top
                    topMargin: parent.height - tabRadius - marginTop - tabBorderSize
                    horizontalCenter: parent.horizontalCenter
                }
                width: tabBar.currentTab === 0 ?  parent.width - tabBorderSize * 2 - 2 : parent.width + tabBorderSize * 2
                height: tabBorderSize
                //visible: tabBar.currentTab === 0
                //color: tabBar.currentTab === 0 ? "darkGray" : "gray"
                color: tabBar.currentTab === 0 ? "white" : "black"
                //color: "gray"
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: tabRadius + marginTop
                color: "white"
                width: parent.width + tabBorderSize
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
                width: 175
                visible: tabBar.height > 0
                height: tabBar.height - marginTop
                //border.width: tabBar.currentTab === index + 1 ? 3 : 1
                border.width: tabBorderSize
                border.color: tabBar.currentTab === index + 1 ? "black" : "gray"
                radius: tabRadius
                Text {
                    id: text
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.topMargin: 20
                    //verticalAlignment: Text.AlignVCenter
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignLeft
                    color: tabBar.currentTab === index + 1 ? "black" : "gray"
                    text: modelData
                }
                Rectangle {
                    anchors {
                        top: parent.top
                        topMargin: parent.height - tabRadius - marginTop - tabBorderSize
                        horizontalCenter: parent.horizontalCenter
                    }
                    //width: parent.width - tabBorderSize * 2
                    width: tabBar.currentTab === index + 1 ? parent.width - tabBorderSize * 2 - 2 : parent.width + tabBorderSize * 2
                    height: tabBorderSize
                    //height: tabBar.currentTab === index + 1 ? parent.radius : 1
                    //color: tabBar.currentTab === index + 1 ? "darkGray" : "gray"
                    color: tabBar.currentTab === index + 1 ? "white" : "black"
                    //color: "red"
                    //opacity: 0.9
                    //visible: tabBar.currentTab === index + 1
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        tabBar.currentTab = index + 1
                    }
                }

                Rectangle {
                    //anchors.left: parent.left
                    //anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: tabRadius + marginTop
                    color: "white"
                    //opacity: 0.5
                    width: parent.width// - (tabRepeater.count) * 175 - 96
                }


                Rectangle {
                    height: 30
                    width: 30
                    radius: height / 2
                    border.color: tabBar.currentTab === index + 1 ? "black" : "gray"
                    border.width: 1
                    anchors {
                        right: parent.right
                        top: parent.top
                        topMargin: (parent.height - tabRadius - marginTop) / 2 - height / 2
                        rightMargin: (parent.height - tabRadius - marginTop) / 2 - height / 2
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "X"
                        font.pointSize: 14
                        color: tabBar.currentTab === index + 1 ? "black" : "gray"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tabBar.currentTab = 0

                            tabBar.objectList[index].destroy()
                            tabBar.objectList.splice(index, 1)

                            var tabModel = tabBar.tabModel
                            tabModel.splice(index, 1)
                            tabBar.tabModel = tabModel
                        }
                    }
                }
            }
        }
    }

    Image {
        id: batteryIcon

        width: 45
        height: 45//parent.height - (tabRadius + marginTop + 1)
        //visible: width > 0
        //color: "red"
        anchors {
            right: rotateButton.left
            rightMargin: 10
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: - (tabRadius + marginTop) / 2
        }
        source: SystemMonitor.batteryLeft > 10 ?
                    (SystemMonitor.batteryLeft > 50 ?
                         (SystemMonitor.batteryLeft > 90 ? "qrc:/icons/batt_max.svg" : "qrc:/icons/Batt med-high.svg") :
                         "qrc:/icons/batt med.svg") :
                    "qrc:/icons/batt low.svg"

        rotation: 90

        MouseArea {
            anchors.fill: parent
            onClicked: debugWindow.visible = !debugWindow.visible
        }
    }

    Image {
        id: rotateButton
        source: rootItem.rotation === 0 ? "qrc:/icons/Rotate_lying.svg" : "qrc:/icons/Rotate_standing.svg"
        anchors {
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: - (tabRadius + marginTop - 1) / 2
            right: parent.right
            rightMargin: 15
        }
        height: 40
        width: height

        MouseArea {
            anchors.fill: parent
            onClicked: rootItem.rotateScreen()
        }
    }
}
