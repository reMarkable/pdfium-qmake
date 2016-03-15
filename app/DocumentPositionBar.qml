import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: positionBar
    property int pageCount: 0
    property int currentPage: 0

    signal pageClicked(var page)

    height: 50
    
    Item {
        anchors.fill: parent

        visible: positionBar.pageCount >= 100

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 10

            border.width: 1
            color: "white"
            radius: 2

            Rectangle {
                anchors {
                    left: parent.left
                    leftMargin: 1
                    verticalCenter: parent.verticalCenter
                }
                height: parent.height - 2
                width: positionBar.pageCount > 0 ? parent.width * (positionBar.currentPage / positionBar.pageCount) : 0
                color: "#aaa"
                radius: 2
            }
        }
        MouseArea {
            anchors.fill: parent
            enabled: parent.visible
            onClicked: {
                positionBar.pageClicked(Math.floor(positionBar.pageCount * mouse.x / width))
            }
        }
    }
    
    Row {
        id: positionRow
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        
        visible: positionBar.pageCount < 100 && positionBar.pageCount > 1
        
        Repeater {
            id: positionRepeater
            model: visible ? positionBar.pageCount : 0
            delegate: Item {
                height: positionBar.height
                width: Math.min((positionBar.width - 400) / positionRepeater.count - positionRow.spacing, 50)
                
                Rectangle {
                    anchors {
                        right: parent.right
                        rightMargin: 1
                        left: parent.left
                        leftMargin: 1
                        bottom: parent.bottom
                    }
                    border.width: 1
                    
                    height: 14
                    color: positionBar.currentPage < index ? "white" : "#aaa"
                    radius: 2
                }
                MouseArea {
                    anchors.fill: parent
                    enabled: parent.visible
                    onClicked: positionBar.pageClicked(index)
                }
            }
        }
    }
}
