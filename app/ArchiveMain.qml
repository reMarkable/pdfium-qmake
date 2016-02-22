import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveMain
    visible: archiveView.currentBook === ""
    
    property int currentPage: 0
    onCurrentPageChanged: mainArchiveGrid.reloadDocuments()

    property int documentPreviewHeight: 380
    property int documentPreviewWidth: 300

    property bool selectionModeActive: false

    onSelectionModeActiveChanged: {
        selectedBooks = []
    }

    function deleteBooks() {
        if (selectedBooks.length === 0) {
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
            pageRowRepeater.reloadModel()
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
            delegate: BookThumbnail {
                id: bookItem

                property bool selected: (archiveMain.selectedBooks.indexOf(modelData) !== -1)
                showTools: !archiveMain.selectionModeActive

                MouseArea {
                    id: selectionMouseArea
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
                            archiveView.currentBook = modelData
                        }
                    }
                }

                Rectangle {
                    id: bookSelectionOverlay
                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    color: "#7f000000"
                    visible: archiveMain.selectionModeActive && !bookItem.selected
                    width: archiveMain.documentPreviewWidth + 2
                    height: archiveMain.documentPreviewHeight + 2
                }

                Image {
                    id: bookSelectedIcon
                    anchors.centerIn: parent
                    width: mainScreen.bigIconSize
                    height: width

                    sourceSize.width: width
                    sourceSize.height: width
                    visible: archiveMain.selectionModeActive
                    source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
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
        
        spacing: 20
        visible: pageRowRepeater.count > 1
        
        Repeater {
            id: pageRowRepeater
            function reloadModel() {
                model = Collection.localDocumentCount() > mainArchiveGrid.pageItemCount ? Math.ceil(Collection.localDocumentCount() / mainArchiveGrid.pageItemCount) : 0
            }

            onCountChanged: {
                if (archiveMain.currentPage >= count) {
                    archiveMain.currentPage = count
                }
            }

            delegate: Rectangle {
                width: 30
                height: width
                radius: 2
                color: archiveMain.currentPage === index ? "black" : "gray"

                MouseArea {
                    anchors.fill: parent
                    onClicked: archiveMain.currentPage = index
                }
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
        color: "#666"
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
        color: "#666"
        onClicked: archiveMain.currentPage--
    }

    Dialog {
        id: deleteDialog
        onAccepted: {
            for (var i=0; i<archiveMain.selectedBooks.length; i++) {
                Collection.deleteDocument(archiveMain.selectedBooks[i])
                tabBar.closeDocument(archiveMain.selectedBooks[i])
            }

            archiveMain.selectionModeActive = false
            mainArchiveGrid.reloadDocuments()
        }
    }
}
