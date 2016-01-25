import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: header
    anchors {
        bottom: parent.top
        right: parent.right
        left: parent.left
    }

    property string text
    height: 3
    color: "gray"
    
    Text {
        id: headerText
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
            bottomMargin: 10
        }
        color: window.fontColor
        text: header.text
        font.pointSize: 18
        font.bold: true
    }
    
    
    Image {
        anchors {
            right: parent.right
            bottom: parent.top
            bottomMargin: 10
        }
        
        height: headerText.height
        width: height
        source: "qrc:/icons/forward.svg"
        smooth: true
    }
}
