import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: bookItem
    width: 320
    height: 400

    signal clicked()

    MouseArea {
        id: selectionMouseArea
        anchors.fill: parent
        onClicked: bookItem.clicked()
    }

    Repeater {
        id: bgPageRepeater
        model: Math.min(Collection.pageCount(modelData), 5) - 1
        property real spacing: Math.max(20 / count, 3)
        Rectangle {
            border.width: 1
            width: bookItem.width - 20
            height: bookItem.height - 20
            x: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
            y: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
        }
    }
    
    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
        }
        width: parent.width - 20
        height: parent.height - 20
        
        border.width: 1
        
        Image {
            id: documentThumbnail
            anchors.centerIn: parent

            asynchronous: true
            width: parent.width - 4
            height: parent.height - 4
            sourceSize.width: width
            sourceSize.height: height
            
            source: Collection.thumbnailPath(modelData)
        }
        
        Rectangle {
            id: bookHeader
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 1
            }
            height: 50
            color: "#b0ffffff"
            Image {
                id: bookIcon
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    margins: 5
                }
                width: height
                source: "qrc:///icons/Notebook.svg"
                sourceSize.width: width
                sourceSize.height: height
            }

            Text {
                anchors {
                    left: bookIcon.right
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                    right: pageCountLabel.left
                }
                elide: Text.ElideRight
                text: Collection.title(modelData)
            }

            Text {
                id: pageCountLabel
                anchors {
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                
                text: Collection.pageCount(modelData)
            }
        }
    }
}
