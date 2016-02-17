import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: archiveButton

    width: 75
    height: width
    color: "#a0000000"
    radius: 5

    property url icon
    signal clicked
    
    Image {
        anchors.fill: parent
        anchors.margins: 10
        sourceSize.width: width
        sourceSize.height: width
        source: archiveButton.icon
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: archiveButton.clicked()
    }
}
