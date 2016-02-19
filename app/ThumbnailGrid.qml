import QtQuick 2.5
import com.magmacompany 1.0

Rectangle {
    id: thumbnailGrid
    width: rootItem.width
    height: rootItem.height
    x: -viewRoot.x
    y: -viewRoot.y
    visible: false
    
    color: "#7f000000"
    
    property int maxDisplayItems: 9
    property int currentThumbnailPage
    property int thumbnailPageCount
    property int pageCount
    property string documentPath
    
    MouseArea {
        anchors.fill: parent
        enabled: thumbnailGrid.visible
        onClicked: thumbnailGrid.visible = false
    }
    
        Image {
            anchors {
                top: parent.top
                right: grid9Select.left
                topMargin: 75
                rightMargin: 10
            }
            height: 75
            width: height
            source: "qrc:///icons/yes_white.svg"
        }
    
    Image {
        id: grid9Select
        anchors {
            top: parent.top
            right: grid36Select.left
            topMargin: 75
            rightMargin: 5
        }

        height: 75
        width: height
        source: thumbnailGrid.maxDisplayItems === 9 ? "qrc:///icons/Grid 9.svg" : "qrc:///icons/Grid 9_white.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: thumbnailGrid.maxDisplayItems = 9
        }
    }

    Image {
        id: grid36Select
        anchors {
            top: parent.top
            right: parent.right
            topMargin: 75
            rightMargin: 5
        }
        height: 75
        width: height
        source: thumbnailGrid.maxDisplayItems === 36 ? "qrc:///icons/Grid 36.svg" : "qrc:///icons/Grid 36_white.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: thumbnailGrid.maxDisplayItems = 36
        }
    }
    
    Grid {
        id: thumbGrid
        anchors {
            left: parent.left
            leftMargin: 150
            top: parent.top
            topMargin: 200
        }
        columns: (thumbnailGrid.maxDisplayItems === 9 ) ? 3 : 6
        rows: (thumbnailGrid.maxDisplayItems === 9 ) ? 3 : 6
        
        spacing: (thumbnailGrid.maxDisplayItems === 9) ? 50 : 10
        
        Repeater {
            model: thumbnailGrid.visible ? Math.min(thumbnailGrid.maxDisplayItems, thumbnailGrid.pageCount - thumbnailGrid.currentThumbnailPage * thumbnailGrid.maxDisplayItems) : 0
            
            delegate: Rectangle {
                id: thumbnailDelegate
                width: (thumbnailGrid.maxDisplayItems === 9 ) ? 270 : 150
                height: (thumbnailGrid.maxDisplayItems === 9 ) ? 370 : 200
                color: "white"
                property int pageNumber: modelData + thumbnailGrid.currentThumbnailPage * thumbnailGrid.maxDisplayItems
                border.color: "black"
                border.width: thumbnailGrid.currentThumbnailPage === pageNumber ? 8 : 0
                
                Image {
                    anchors.fill: parent
                    anchors.margins: 8
                    source: "file://" + thumbnailGrid.documentPath + "-" + thumbnailDelegate.pageNumber + ".thumbnail.jpg"
                    smooth: false
                    asynchronous: true
                    
                    Rectangle {
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        width: 50
                        height: 50
                        color: "#a0000000"
                        radius: 5
                        
                        Text {
                            anchors.centerIn: parent
                            color: "white"
                            text: (thumbnailDelegate.pageNumber + 1) + "/" + thumbnailGrid.pageCount
                        }
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        document.currentIndex = thumbnailDelegate.pageNumber
                        thumbnailGrid.visible = false
                    }
                }
            }
        }
    }
    
    
    Item {
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
            margins: 50
        }
        height: 75

        MouseArea {
            anchors.fill: parent
        }
        
        Row {
            id: pageRow
            anchors.centerIn: parent
            
            spacing: 10
            
            Repeater {
                model: thumbnailGrid.thumbnailPageCount
                delegate: Rectangle {
                    width: 30
                    height: width
                    radius: 2
                    color: thumbnailGrid.currentThumbnailPage === index ? "black" : "transparent"
                    border.width: 2
                    border.color: "white"
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: thumbnailGrid.currentThumbnailPage = index
                    }
                }
            }
        }
        
        
        Image {
            anchors.right: parent.right
            width: height
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            source: "qrc:///icons/forward_white.svg"
            visible: (thumbnailGrid.currentThumbnailPage < thumbnailGrid.thumbnailPageCount - 1)
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (thumbnailGrid.currentThumbnailPage < thumbnailGrid.thumbnailPageCount - 1) {
                        thumbnailGrid.currentThumbnailPage++
                    }
                }
            }
        }
        
        Image {
            anchors.left: parent.left
            width: height
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            source: "qrc:///icons/back_white.svg"
            visible: (thumbnailGrid.currentThumbnailPage > 0)
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (thumbnailGrid.currentThumbnailPage > 0) {
                        thumbnailGrid.currentThumbnailPage--
                    }
                }
            }
        }
    }
}
