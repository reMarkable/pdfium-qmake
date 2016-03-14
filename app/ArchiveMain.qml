import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveMain
    visible: archiveView.currentBook === ""
    
    property int currentPage: 0
    onCurrentPageChanged: reloadTimer.start()

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
        onMaxDisplayItemCountChanged: pageRowRepeater.reloadModel()
    }

    Grid {
        id: mainArchiveGrid
        anchors {
            top: editActionsItem.bottom
            bottom: parent.bottom
            topMargin: 25
            horizontalCenter: parent.horizontalCenter
        }

        columns: (editActionsItem.maxDisplayItemCount === 9 ) ? 3 : 6
        rows: (editActionsItem.maxDisplayItemCount === 9 ) ? 3 : 6
        spacing: 30

        property int pageItemCount: columns * rows
        
        function reloadDocuments() {
            pageRowRepeater.reloadModel()
            documentRepeater.model = Collection.getDocumentPaths(mainArchiveGrid.pageItemCount, archiveMain.currentPage * mainArchiveGrid.pageItemCount)
        }

        Component.onCompleted: reloadTimer.start()
        Connections {
            target: Collection
            onDocumentPathsChanged: {
                mainArchiveGrid.reloadDocuments()
            }
        }
        Timer {
            id: reloadTimer
            interval: 0
            running: false
            repeat: false
            onTriggered: {
                parent.reloadDocuments()
            }

        }

        Repeater {
            id: documentRepeater
            delegate: BookThumbnail {
                id: bookItem

                property bool isDefault: (modelData.indexOf("Default ") !== -1)

                width: (editActionsItem.maxDisplayItemCount === 9 ) ? 320 : 150
                height: (editActionsItem.maxDisplayItemCount === 9 ) ? 400 : 200

                property bool selected: (archiveMain.selectedBooks.indexOf(modelData) !== -1)

                onClicked: {
                    if (editActionsItem.selectionModeActive) {
                        // Default books can't be deleted
                        if (isDefault) {
                            return;
                        }

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
                    visible: !isDefault && editActionsItem.selectionModeActive && !bookItem.selected
                    width: parent.width - 20
                    height: parent.height - 20

                }

                Image {
                    id: bookSelectedIcon
                    anchors.centerIn: bookSelectionOverlay
                    width: mainScreen.bigIconSize
                    height: width
                    asynchronous: true

                    sourceSize.width: width
                    sourceSize.height: width
                    visible: !isDefault && editActionsItem.selectionModeActive
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
                model = Collection.documentCount() > editActionsItem.maxDisplayItemCount ? Math.ceil(Collection.documentCount() / editActionsItem.maxDisplayItemCount) : 0
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
        icon: "qrc:///icons/forward.svg"
        color: "white"
        onClicked: archiveMain.currentPage++
    }
    
    ArchiveButton {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: 50
        }
        visible: archiveMain.currentPage > 0
        icon: "qrc:///icons/back.svg"
        color: "white"
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
