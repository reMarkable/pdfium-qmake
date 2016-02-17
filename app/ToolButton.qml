import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: toolButton
    width: 64
    height: width
    radius: 5

    property string icon
    property bool active: false
    signal clicked
    
    border.width: 4
    border.color: toolButton.active ? "gray" : "white"
    
    Image {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 8
        width: parent.width - 16
        height: width
        source: toolButton.icon
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            toolButton.clicked()
        }
    }
}
