import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveBook
    
    visible: archiveView.currentBook != ""
    property QtObject document
    
    onVisibleChanged: {
        if (!visible) {
            document = null
            pageRepeater.model = 0
        } else {
            document = Collection.getDocument(archiveView.currentBook)
            document.preload()
            pageRepeater.model = Math.min(document.pageCount, 9)
        }
    }
    
    Grid {
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: 25
            horizontalCenter: parent.horizontalCenter
        }
        columns: 3
        rows: 3
        
        Repeater {
            id: pageRepeater
            
            delegate: Item {
                id: bookItem
                width: 320
                height: 450
                property bool selected: false

                Rectangle {
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        rightMargin: index % 2 == 0 ? -10 : 20
                    }
                    height: 150
                    radius: 5
                    color: "gray"

                    Text {
                        anchors {
                            top: parent.top
                            topMargin: 5
                            left: parent.left
                            leftMargin: 10
                        }
                        visible: index % 2 == 0

                        text: "Header lol"
                        color: "white"
                    }
                }
                
                Rectangle {
                    anchors {
                        fill: parent
                        topMargin: 50
                        bottomMargin: 20
                        rightMargin: 30
                        leftMargin: 30
                    }
                    
                    border.width: 1
                    Image {
                        id: pageThumbnail
                        anchors {
                            fill: parent
                            margins: 2
                        }
                        asynchronous: true
                        
                        source: "file://" + archiveView.currentBook + "-" + index + ".thumbnail.jpg"
                        
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
                                archiveView.openBookAt(archiveView.currentBook, index)
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
                            text: (index + 1)
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
                            anchors.margins: 10
                            source: "qrc:///icons/xoom+_white.svg"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                previewBackground.index = index
                                previewBackground.visible = true
                            }
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
                                anchors.margins: 10
                                source: "qrc:///icons/Lock_small_white.svg"
                            }
                        }
                        Rectangle {
                            width: moreButton.width
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
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
                                anchors.margins: 10
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
                                anchors.margins: 10
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


    Rectangle {
        id: previewBackground
        width: rootItem.width
        height: rootItem.height
        x: - (viewRoot.x + archiveBook.x)
        y: - (viewRoot.y + archiveBook.y)

        visible: false

        color: "#7f000000"

        property int index: 0

        MouseArea {
            anchors.fill: parent
            onClicked: previewBackground.visible = false
        }

        Image {
            id: previewImage
            anchors {
                fill: parent
                margins: 100
            }

            source: visible ? "file://" + archiveView.currentBook + "-" + previewBackground.index + ".thumbnail.jpg" : ""

            Rectangle {
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 50
                }
                width: height
                height: 75
                color: "#a0000000"
                radius: 5

                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:///icons/forward_white.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: previewBackground.index++
                }
            }

            Rectangle {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 50
                }

                width: height
                height: 75
                color: "#a0000000"
                radius: 5

                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:///icons/back_white.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: previewBackground.index--
                }
            }

            Rectangle {
                anchors {
                    top: parent.top
                    left: parent.left
                }

                width: height
                height: 75
                color: "#a0000000"
                radius: 5

                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:///icons/Delete_white.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: console.log("pls implement")
                }
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                }

                width: height
                height: 75
                color: "#a0000000"
                radius: 5

                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:///icons/Open-book_white.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: archiveView.openBookAt(archiveView.currentBook, previewBackground.index)
                }
            }


            Grid {
                id: previewToolGrid
                anchors {
                    left: parent.left
                    bottom: previewMoreButton.top
                    bottomMargin: 1
                }
                rows: 2
                columns: 2
                spacing: 1
                visible: false

                Rectangle {
                    width: previewMoreButton.width
                    height: width
                    color: "#a0000000"
                    radius: 5

                    Image {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: "qrc:///icons/Lock_small_white.svg"
                    }
                }
                Rectangle {
                    width: previewMoreButton.width
                    height: width
                    color: "#a0000000"
                    radius: 5

                    Image {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: "qrc:///icons/send_white.svg"
                    }
                }
                Rectangle {
                    width: previewMoreButton.width
                    height: width
                    color: "#a0000000"
                    radius: 5

                    Image {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: "qrc:///icons/Move_white.svg"
                    }
                }
                Rectangle {
                    width: previewMoreButton.width
                    height: width
                    color: "#a0000000"
                    radius: 5

                    Image {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: "qrc:///icons/Delete_white.svg"
                    }
                }
            }

            Rectangle {
                id: previewMoreButton
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                }

                width: 75
                height: width
                color: "#a0000000"
                radius: 5

                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:///icons/prikkprikkprikk_white.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: previewToolGrid.visible = !previewToolGrid.visible
                }
            }
        }
    }
}
