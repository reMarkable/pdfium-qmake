import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: mainArchive
    visible: archiveView.currentBook === ""
    
    property int currentPage: 0

    property int smallIconSize: 75
    property int mediumIconSize: 200
    property int documentPreviewHeight: 380
    property int documentPreviewWidth: 300

    function deleteBook(path) {
        deleteDialog.documentPath = path
    }
    
    Grid {
        id: mainArchiveGrid
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: 25
            horizontalCenter: parent.horizontalCenter
        }
        columns: 3
        rows: 3
        spacing: 30

        property int pageItemCount: columns * rows
        
        function reloadDocuments() {
            documentRepeater.model = Collection.recentlyUsedPaths(mainArchiveGrid.pageItemCount, mainArchive.currentPage * mainArchiveGrid.pageItemCount)
        }

        Component.onCompleted: reloadDocuments()
        Connections {
            target: Collection
            onRecentlyUsedChanged: {
                mainArchiveGrid.reloadDocuments()
            }
        }

        Repeater {
            id: documentRepeater
            delegate: Item {
                id: bookItem
                width: 320
                height: 400
                property bool selected: false
                
                Repeater {
                    id: bgPageRepeater
                    model: Math.min(Collection.pageCount(modelData), 5) - 1
                    property int spacing: Math.max(20 / count, 3)
                    Rectangle {
                        border.width: 1
                        width: mainArchive.documentPreviewWidth
                        height: mainArchive.documentPreviewHeight
                        x: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
                        y: bgPageRepeater.count * bgPageRepeater.spacing - modelData * bgPageRepeater.spacing
                    }
                }
                
                Rectangle {
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: mainArchive.documentPreviewWidth + 2
                    height: mainArchive.documentPreviewHeight + 2

                    border.width: 1

                    Image {
                        id: documentThumbnail

                        anchors {
                            centerIn: parent
                        }
                        asynchronous: true
                        width: mainArchive.documentPreviewWidth
                        height: mainArchive.documentPreviewHeight
                        sourceSize.width: width
                        sourceSize.height: height
                        
                        source: Collection.thumbnailPath(modelData)
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "#7f000000"
                            visible: editActionsRow.visible && !bookItem.selected
                        }
                        
                        Image {
                            anchors.centerIn: parent
                            width: mainArchive.mediumIconSize
                            height: width

                            sourceSize.width: width
                            sourceSize.height: width
                            visible: editActionsRow.visible
                            source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (editActionsRow.visible) {
                                    bookItem.selected = !bookItem.selected
                                } else {
                                    archiveView.openBook(modelData)
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
                        
                        width: mainArchive.smallIconSize
                        height: width
                        color: "#a0000000"
                        radius: 5
                        
                        Image {
                            anchors.fill: parent
                            anchors.margins: 10
                            sourceSize.width: width
                            sourceSize.height: width
                            source: "qrc:///icons/forward_white.svg"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                archiveView.currentBook = modelData
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
                            width: mainArchive.smallIconSize
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                sourceSize.width: width
                                sourceSize.height: width
                                source: "qrc:///icons/Lock_small_white.svg"
                            }
                        }
                        Rectangle {
                            width: mainArchive.smallIconSize
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                sourceSize.width: width
                                sourceSize.height: width
                                source: "qrc:///icons/send_white.svg"
                            }
                        }

                        Rectangle {
                            width: mainArchive.smallIconSize
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                sourceSize.width: width
                                sourceSize.height: height
                                source: "qrc:///icons/Move_white.svg"
                            }
                        }
                        Rectangle {
                            width: mainArchive.smallIconSize
                            height: width
                            color: "#a0000000"
                            radius: 5
                            
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                sourceSize.width: width
                                sourceSize.height: width
                                source: "qrc:///icons/Delete_white.svg"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: mainArchive.deleteBook(modelData)
                            }
                        }
                    }
                    
                    Rectangle {
                        id: moreButton
                        anchors {
                            bottom: parent.bottom
                            left: parent.left
                        }
                        
                        width: mainArchive.smallIconSize
                        height: width
                        color: "#a0000000"
                        radius: 5
                        
                        Image {
                            anchors.centerIn: parent
                            width: mainArchive.smallIconSize
                            height: width
                            sourceSize.width: width
                            sourceSize.height: width
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
            id: pageRowRepeater
            model: Math.ceil(Collection.localDocumentCount() / mainArchiveGrid.pageItemCount)
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
        visible: mainArchive.currentPage < pageRowRepeater.count - 1
        
        Image {
            anchors.fill: parent
            anchors.margins: 5
            source: "qrc:///icons/forward_white.svg"
            sourceSize.width: width
            sourceSize.height: height
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
        visible: mainArchive.currentPage > 0
        
        Image {
            anchors.fill: parent
            anchors.margins: 5
            source: "qrc:///icons/back_white.svg"
            sourceSize.width: width
            sourceSize.height: height
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: mainArchive.currentPage--
        }
    }


    Rectangle {
        width: rootItem.width
        height: rootItem.height
        x: - (viewRoot.x + mainArchive.x)
        y: - (viewRoot.y + mainArchive.y)
        visible: deleteDialog.documentPath !== ""

        color: "#7f000000"

        Rectangle {
            id: deleteDialog
            anchors.centerIn: parent
            width: 750
            height: 400
            border.width: 5
            radius: 10
            color: "white"

            property string documentPath: ""

            Text {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 75
                    bottom: parent.verticalCenter
                }

                text: "Are you sure you want to delete " + Collection.title(deleteDialog.documentPath) + "?"
                font.pointSize: 18
                wrapMode: Text.Wrap
            }

            Item {
                anchors {
                    right: parent.horizontalCenter
                    left: parent.left
                    bottom: parent.bottom
                    top: parent.verticalCenter
                }

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/icons/yes.svg"
                    height: 100
                    width: height
                    MouseArea {
                        enabled: deleteDialog.visible
                        anchors.fill: parent
                        onClicked: {
                            Collection.deleteDocument(deleteDialog.documentPath)
                            deleteDialog.documentPath = ""
                        }
                    }
                }
            }

            Item {
                anchors {
                    left: parent.horizontalCenter
                    right: parent.right
                    bottom: parent.bottom
                    top: parent.verticalCenter
                }

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/icons/no.svg"
                    height: 100
                    width: height
                    MouseArea {
                        enabled: deleteDialog.visible
                        anchors.fill: parent
                        onClicked: {

                            deleteDialog.documentPath = ""
                        }
                    }
                }
            }
        }
    }
}
