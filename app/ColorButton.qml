import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: colorButton
    width: parent.width
    height: width
    radius: width / 2

    property color highlightColor
    property bool active
    signal clicked

    border.width: 2
    border.color: "black"
    
    Rectangle {
        anchors.centerIn: parent
        width: 20
        height: width
        radius: width / 2
        color: colorButton.active ? colorButton.highlightColor : colorButton.color
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            colorButton.clicked()
        }
    }
}
