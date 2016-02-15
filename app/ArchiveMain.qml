import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: mainArchive
    visible: archiveView.currentBook === ""
    anchors {
        top: archiveHeader.bottom
        bottom: parent.bottom
        left: parent.left
        right: parent.right
    }
    
    property int currentPage: 0
    
    Grid {
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: 25
            horizontalCenter: parent.horizontalCenter
        }
        columns: 3
        rows: 3
        spacing: 30
        
        Repeater {
            model: Collection.folderEntries()
            
            delegate: Item {
                id: bookItem
                width: 320
                height: 450
                property bool selected: false
                
                Repeater {
                    id: bgPageRepeater
                    model: Math.min(Collection.pageCount(modelData), 5) - 1
                    property int spacing: Math.max(20 / count, 3)
                    Rectangle {
                        border.width: 1
                        width: 300
                        height: 430
                        x: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
                        y: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
                    }
                }
                
                Rectangle {
                    anchors {
                        fill: parent
                        bottomMargin: 20
                        rightMargin: 20
                    }
                    
                    border.width: 1
                    Image {
                        anchors {
                            fill: parent
                            margins: 2
                        }
                        asynchronous: true
                        
                        source: "file://" + Collection.thumbnailPath(modelData)
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "#7f000000"
                            visible: editActionsRow.visible && !bookItem.selected
                        }
                        
                        Image {
                            anchors.centerIn: parent
                            width: parent.width / 2
                            height: width
                            visible: editActionsRow.visible
                            source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (editActionsRow.visible) {
                                    bookItem.selected = !bookItem.selected
                                } else {
                                    archiveView.currentBook = modelData
                                }
                            }
                        }
                    }
                    
                    Rectangle {
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                            rightMargin: 60
                        }
                        height: 50
                        color: "#b0ffffff"
                        border.width: 1
                        Image {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                                left: parent.left
                                margins: 5
                            }
                            width: height
                            source: "qrc:///icons/Notebook.svg"
                            
                            Text {
                                anchors {
                                    left: parent.right
                                    leftMargin: 10
                                    verticalCenter: parent.verticalCenter
                                }
                                text: Collection.title(modelData)
                            }
                        }
                        
                    }
                    
                    Rectangle {
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        width: 50
                        height: 50
                        color: "#b0ffffff"
                        border.width: 1
                        
                        Text {
                            anchors.centerIn: parent
                            text: Collection.pageCount(modelData)
                        }
                    }
                    
                    Rectangle {
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }
                        
                        width: parent.width / 3
                        height: width
                        color: "#a0000000"
                        radius: 5
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 5
                            source: "qrc:///icons/forward_white.svg"
                        }
                    }
                    
                    
                    Grid {
                        id: toolGrid
                        anchors {
                            left: parent.left
                            bottom: moreButton.top
                            bottomMargin: 1
                        }
                        rows: 2
                        columns: 2
                        spacing: 1
                        visible: false
                        
                        Rectangle {
                            width: moreButton.width
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                source: "qrc:///icons/Move_white.svg"
                            }
                        }
                        Rectangle {
                            width: moreButton.width
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                source: "qrc:///icons/send_white.svg"
                            }
                        }
                        Rectangle {
                            width: moreButton.width
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                source: "qrc:///icons/Move_white.svg"
                            }
                        }
                        Rectangle {
                            width: moreButton.width
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                source: "qrc:///icons/Delete_white.svg"
                            }
                        }
                    }
                    
                    Rectangle {
                        id: moreButton
                        anchors {
                            bottom: parent.bottom
                            left: parent.left
                        }
                        
                        width: parent.width / 3
                        height: width
                        color: "#a0000000"
                        radius: 5
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 5
                            source: "qrc:///icons/prikkprikkprikk_white.svg"
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: toolGrid.visible = !toolGrid.visible
                        }
                    }
                }
            }
        }
    }
    
    Row {
        id: pageRow
        anchors {
            bottom: parent.bottom
            bottomMargin: 80
            horizontalCenter: parent.horizontalCenter
        }
        
        spacing: 10
        
        Repeater {
            model: 5
            delegate: Rectangle {
                width: 20
                height: width
                radius: 2
                color: mainArchive.currentPage === index ? "black" : "gray"
            }
        }
    }
    
    Rectangle {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 50
        }
        width: height
        height: 75
        color: "#a0000000"
        radius: 5
        
        Image {
            anchors.fill: parent
            anchors.margins: 5
            source: "qrc:///icons/forward_white.svg"
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: mainArchive.currentPage++
        }
    }
    
    Rectangle {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: 50
        }
        
        width: height
        height: 75
        color: "#a0000000"
        radius: 5
        
        Image {
            anchors.fill: parent
            anchors.margins: 5
            source: "qrc:///icons/back_white.svg"
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: mainArchive.currentPage--
        }
    }
}
