import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveMain
    visible: archiveView.currentBook === ""
    
    property int currentPage: 0
    onCurrentPageChanged: mainArchiveGrid.reloadDocuments()

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

    /// Key handling
    Component.onCompleted: forceActiveFocus()
    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        }
    }
    Keys.onRightPressed: {
        if (currentPage < pageRowRepeater.count - 1) {
            currentPage++
        }
    }
    Keys.onLeftPressed: {
        if (currentPage > 0) {
            currentPage--
        }
    }

    EditActions {
        id: editActionsItem
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 25
            rightMargin: 50
        }
        onSelectionModeActiveChanged: {
            selectedBooks = []
        }
        onDeleteItems: {
            deleteBooks()
        }
    }

    Grid {
        id: mainArchiveGrid
        anchors {
            top: editActionsItem.bottom
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

                onClicked: {
                    if (editActionsItem.selectionModeActive) {
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

                Rectangle {
                    id: bookSelectionOverlay
                    anchors {
                        top: parent.top
                        left: parent.left
                    }

                    color: "#7f000000"
                    visible: editActionsItem.selectionModeActive && !bookItem.selected
                    width: parent.width - 20
                    height: parent.height - 20

                }

                Image {
                    id: bookSelectedIcon
                    anchors.centerIn: bookSelectionOverlay
                    width: mainScreen.bigIconSize
                    height: width

                    sourceSize.width: width
                    sourceSize.height: width
                    visible: editActionsItem.selectionModeActive
                    source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                }


                ArchiveButton {
                    id: showBookButton
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 20
                        right: parent.right
                        rightMargin: 20
                    }
                    visible: !editActionsItem.selectionModeActive
                    icon: "qrc:///icons/Open-book_white.svg"
                    onClicked: archiveView.openBook(modelData)
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

            editActionsItem.selectionModeActive = false
            mainArchiveGrid.reloadDocuments()
        }
    }
}
