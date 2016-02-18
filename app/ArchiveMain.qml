import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveMain
    visible: archiveView.currentBook === ""
    
    property int currentPage: 0
    onCurrentPageChanged: mainArchiveGrid.reloadDocuments()

    property int smallIconSize: 75
    property int mediumIconSize: 200
    property int documentPreviewHeight: 380
    property int documentPreviewWidth: 300

    property bool selectionModeActive: false

    onSelectionModeActiveChanged: {
        selectedBooks = []
    }

    function deleteBooks() {
        if (selectedBooks.count === 0) {
            return
        }
        deleteDialog.show("Are you sure you want to delete these books?")
    }

    function deleteBook(path) {
        if (path === "") {
            return
        }

        selectedBooks = [path]
        deleteDialog.show("Are you sure you want to delete " + Collection.title(deleteDialog.documentPath) + "?")
    }

    property var selectedBooks: []
    
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
            documentRepeater.model = Collection.recentlyUsedPaths(mainArchiveGrid.pageItemCount, archiveMain.currentPage * mainArchiveGrid.pageItemCount)
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
                property bool selected: (archiveMain.selectedBooks.indexOf(modelData) !== -1)
                
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

                        anchors {
                            centerIn: parent
                        }
                        asynchronous: true
                        width: archiveMain.documentPreviewWidth
                        height: archiveMain.documentPreviewHeight
                        sourceSize.width: width
                        sourceSize.height: height
                        
                        source: Collection.thumbnailPath(modelData)

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (archiveMain.selectionModeActive) {
                                    var selectedBooks = archiveMain.selectedBooks
                                    if (bookItem.selected) {
                                        selectedBooks.splice(selectedBooks.indexOf(modelData), 1)
                                    } else {
                                        selectedBooks.push(modelData)
                                    }
                                    archiveMain.selectedBooks = selectedBooks
                                } else {
                                    archiveView.openBook(modelData)
                                }
                            }
                        }

                        ArchiveButton {
                            id: showBookButton
                            anchors {
                                bottom: parent.bottom
                                right: parent.right
                            }
                            visible: !archiveMain.selectionModeActive
                            icon: "qrc:///icons/forward_white.svg"
                            onClicked: archiveView.currentBook = modelData
                        }

                        Rectangle {
                            id: bookSelectionOverlay
                            anchors.fill: parent
                            color: "#7f000000"
                            visible: archiveMain.selectionModeActive && !bookItem.selected
                        }
                        
                        Image {
                            id: bookSelectedIcon
                            anchors.centerIn: parent
                            width: archiveMain.mediumIconSize
                            height: width

                            sourceSize.width: width
                            sourceSize.height: width
                            visible: archiveMain.selectionModeActive
                            source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                        }
                    }
                    
                    Rectangle {
                        id: bookHeader
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
                        id: pageCountLabel
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
                color: archiveMain.currentPage === index ? "black" : "gray"
            }
        }
    }

    ArchiveButton {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 50
        }

        visible: archiveMain.currentPage < pageRowRepeater.count - 1
        icon: "qrc:///icons/forward_white.svg"
        onClicked: archiveMain.currentPage++
    }
    
    ArchiveButton {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: 50
        }
        visible: archiveMain.currentPage > 0
        icon: "qrc:///icons/back_white.svg"
        onClicked: archiveMain.currentPage--
    }

    Dialog {
        id: deleteDialog
        onAccepted: {
            for (var i=0; i<archiveMain.selectedBooks.count; i++) {
                Collection.deleteDocument(archiveMain.selectedBooks[i])
            }

            archiveMain.selectionModeActive = false
        }
    }
}
