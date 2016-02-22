import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: bookItem
    width: 320
    height: 400

    property bool showTools: false
    
    Repeater {
        id: bgPageRepeater
        model: Math.min(Collection.pageCount(modelData), 5) - 1
        property int spacing: Math.max(20 / count, 3)
        Rectangle {
            border.width: 1
            width: archiveMain.documentPreviewWidth
            height: archiveMain.documentPreviewHeight
            x: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
            y: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
        }
    }
    
    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
        }
        width: archiveMain.documentPreviewWidth + 2
        height: archiveMain.documentPreviewHeight + 2
        
        border.width: 1
        
        Image {
            id: documentThumbnail
            anchors.centerIn: parent

            asynchronous: true
            width: archiveMain.documentPreviewWidth
            height: archiveMain.documentPreviewHeight
            sourceSize.width: width
            sourceSize.height: height
            
            source: Collection.thumbnailPath(modelData)
            
            ArchiveButton {
                id: showBookButton
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                }
                visible: bookItem.showTools
                icon: "qrc:///icons/Open-book_white.svg"
                onClicked: archiveView.openBook(modelData)
            }
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
                
                Text {
                    anchors {
                        left: parent.right
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    text: Collection.title(modelData)
                }
            }
            
            Text {
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
