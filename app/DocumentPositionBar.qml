import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: positionBar
    anchors {
        bottom: parent.bottom
        right: parent.right
        rightMargin: 100
        left: parent.left
        leftMargin: 100
    }
    property int pageCount: 0
    property int currentPage: 0
    property QtObject document

    height: 50
    
    Item {
        anchors.fill: parent

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 7

            color: "#999"
            radius: 2

            visible: positionBar.pageCount >= 100

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: positionBar.pageCount > 0 ? parent.width * (positionBar.currentPage / positionBar.pageCount) : 0
                color: "#333"
                radius: 2
            }

        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                positionBar.document.currentIndex = Math.floor(document.pageCount * mouse.x / width)
            }
        }
    }
    
    Row {
        id: positionRow
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        
        spacing: 2
        
        visible: positionBar.pageCount < 100
        
        Repeater {
            id: positionRepeater
            model: visible ? positionBar.pageCount : 0
            delegate: Item {
                height: positionBar.height
                width: Math.min((positionBar.width - 400) / positionRepeater.count - positionRow.spacing, 50)
                
                Rectangle {
                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }
                    
                    height: 7
                    color: positionBar.currentPage < index ? "#999" : "#333"
                    radius: 2
                }
                MouseArea {
                    anchors.fill: parent
                    enabled: parent.visible
                    onClicked: positionBar.document.currentIndex = index
                }
            }
        }
    }
}
