import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveBook
    
    signal pageClicked(var index)

    property QtObject document
    onDocumentChanged: {
        if (!document) {
            pageCount = 0
            pageRepeater.model = 0
        } else {
            pageCount = Qt.binding(function() { return Math.ceil(documentPagecount / (thumbnailGrid.rows * thumbnailGrid.columns)); })
            currentPage = 0
            reloadModel()
            canDeletePages = Qt.binding(function() { return ((document.path.indexOf(".pdf") === -1) && documentPagecount > 1); })
        }
    }

    property bool canDeletePages: false

    property var selectedPages: []

    property int currentPage: 0
    property int pageCount: 0
    onPageCountChanged: {
        if (currentPage >= pageCount) {
            currentPage--
        }
    }

    property int documentPagecount: document === null ? 0 : document.pageCount
    onDocumentPagecountChanged: {
        console.log(documentPagecount)
        reloadModel()
    }

    function reloadModel() {
        if (!document) {
            return
        }

        var visiblePages = Math.min(documentPagecount - currentPage * editActionsItem.maxDisplayItemCount, editActionsItem.maxDisplayItemCount)
        if (visiblePages != pageRepeater.count) {
            pageRepeater.model = 0
            pageRepeater.model = visiblePages
        }
    }


    onCurrentPageChanged: {
        console.log("reloading")
        reloadModel()
    }

    /// Key handling
    Keys.onRightPressed: {
        if (currentPage < pageCount - 1) {
            currentPage++
        }
    }
    Keys.onLeftPressed: {
        if (currentPage > 0) {
            currentPage--
        }
    }

    function deletePages() {
        if (selectedPages.length === 0) {
            return
        }

        if (selectedPages.length >= documentPagecount) {
            return
        }

        var pageNumbers = []
        for (var pageNumber in selectedPages) {
            pageNumbers.push(selectedPages[pageNumber] + 1)
        }
        if (selectedPages.length > 5) {
            deleteDialog.show("Are you sure you want to delete these pages?")
        } else if (selectedPages.length > 1) {
            deleteDialog.show("Are you sure you want to delete pages " + pageNumbers.join(", ") + "?")
        } else {
            deleteDialog.show("Are you sure you want to delete page " + pageNumbers[0] + "?")
        }
    }

    function deletePage(index) {
        console.log(index)
        if (index < 0 || index > archiveBook.documentPagecount) {
            return
        }

        selectedPages = [index]
        deleteDialog.show("Are you sure you want to delete page " + (index + 1) + "?")
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
        visible: archiveBook.canDeletePages
        onSelectionModeActiveChanged: {
            selectedPages = []
        }
        onDeleteItems: {
            deletePages()
        }
        onMaxDisplayItemCountChanged: reloadModel()
    }

    Grid {
        id: thumbnailGrid
        anchors {
            top: editActionsItem.bottom
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        columns: (editActionsItem.maxDisplayItemCount === 9 ) ? 3 : 6
        rows: (editActionsItem.maxDisplayItemCount === 9 ) ? 3 : 6

        Repeater {
            id: pageRepeater
            
            delegate: Item {
                id: bookItem
                width: (editActionsItem.maxDisplayItemCount === 9 ) ? 320 : 175
                height: width * 1.25
                property int pageNumber: index + archiveBook.currentPage * editActionsItem.maxDisplayItemCount
                property bool selected: (archiveBook.selectedPages.indexOf(pageNumber) !== -1)

                onPageNumberChanged:{
                    pageThumbnail.reloadThumbnail()
                }

                Connections {
                    target: archiveBook.document
                    onThumbnailUpdated: {
                        if (page !== bookItem.pageNumber) {
                            return
                        }
                        pageThumbnail.reloadThumbnail()
                    }
                }

                Rectangle {
                    anchors {
                        fill: parent
                        topMargin: 15
                        bottomMargin: 15
                        rightMargin: 15
                        leftMargin: 15
                    }
                    
                    border.width: 1
                    Image {
                        id: pageThumbnail
                        anchors {
                            fill: parent
                            margins: 2
                        }
                        asynchronous: true
                        cache: false

                        function reloadThumbnail() {
                            pageThumbnail.source = ""
                            if (!archiveBook.document) {
                                return
                            }
                            pageThumbnail.source = archiveBook.document.getThumbnail(bookItem.pageNumber)
                        }

                        source: archiveBook.document.getThumbnail(bookItem.pageNumber)

                        sourceSize.width: width
                        sourceSize.height: height
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "#7f000000"
                            visible: editActionsItem.selectionModeActive && !bookItem.selected
                        }
                        
                        Image {
                            anchors.centerIn: parent
                            width: parent.width / 2
                            height: width
                            visible: editActionsItem.selectionModeActive
                            source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                            asynchronous: true
                            sourceSize.width: width
                            sourceSize.height: height
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!editActionsItem.selectionModeActive) {
                                    archiveBook.pageClicked(bookItem.pageNumber)
                                    return
                                }

                                var selectedPages = archiveBook.selectedPages
                                if (bookItem.selected) {
                                    selectedPages.splice(selectedPages.indexOf(bookItem.pageNumber), 1)
                                } else {

                                    // Don't allow deleting all pages in document
                                    if (selectedPages.length >= archiveBook.documentPagecount - 1) {
                                        selectedPages.splice(0, 1)
                                    }

                                    selectedPages.push(bookItem.pageNumber)
                                }
                                archiveBook.selectedPages = selectedPages
                            }
                        }
                    }
                    
                    Rectangle {
                        id: pageNumberLabel
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
                            text: (bookItem.pageNumber + 1)
                        }
                    }
                    
                    ArchiveButton {
                        id: previewButton
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }
                        width: (editActionsItem.maxDisplayItemCount === 9 ) ? 75 : 45
                        icon: "qrc:///icons/xoom+_white.svg"
                        visible: !editActionsItem.selectionModeActive

                        onClicked: {
                            previewBackground.index = pageNumber
                            previewBackground.visible = true
                        }
                    }

                    
                    ArchiveButton {
                        id: deleteButton
                        anchors {
                            bottom: parent.bottom
                            left: parent.left
                        }
                        width: (editActionsItem.maxDisplayItemCount === 9 ) ? 75 : 45
                        icon: "qrc:///icons/Delete_white.svg"
                        visible: archiveBook.canDeletePages && !editActionsItem.selectionModeActive
                        
                        onClicked: archiveBook.deletePage(pageNumber)
                    }
                }
            }
        }
    }

    ArchiveButton {
        anchors {
            right: parent.right
            rightMargin: 50
            bottom: parent.bottom
            bottomMargin: 25
        }

        visible: archiveBook.currentPage < archiveBook.pageCount - 1
        icon: "qrc:///icons/forward_white.svg"
        color: "#666"
        onClicked: archiveBook.currentPage++
    }

    ArchiveButton {
        anchors {
            left: parent.left
            leftMargin: 50
            bottom: parent.bottom
            bottomMargin: 25
        }
        visible: archiveBook.currentPage > 0
        icon: "qrc:///icons/back_white.svg"
        color: "#666"
        onClicked: archiveBook.currentPage--
    }

    DocumentPositionBar {
        id: pageRow

        anchors {
            bottom: parent.bottom
            bottomMargin: 60
            right: parent.right
            rightMargin: 200
            left: parent.left
            leftMargin: 200
        }
        pageCount: archiveBook.pageCount
        currentPage: archiveBook.currentPage
        onPageClicked:{
            archiveBook.currentPage = page
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

            source: visible ?  "file://" + archiveBook.document.path + "." + previewBackground.index + ".cached.png" : ""
            sourceSize.width: width
            sourceSize.height: height
            asynchronous: true

            ArchiveButton {
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 50
                }
                icon: "qrc:///icons/forward_white.svg"
                visible: (archiveBook.document !== null) ? (previewBackground.index < archiveBook.documentPagecount - 1) : false
                onClicked: previewBackground.index++
            }

            ArchiveButton {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 50
                }
                icon: "qrc:///icons/back_white.svg"
                visible: previewBackground.index > 0
                onClicked: previewBackground.index--
            }

            ArchiveButton {
                anchors {
                    top: parent.top
                    left: parent.left
                }
                icon: "qrc:///icons/Delete_white.svg"
                onClicked: previewBackground.visible = false
            }

            ArchiveButton {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                }

                icon: "qrc:///icons/Open-book_white.svg"
                onClicked: archiveBook.pageClicked(previewBackground.index)
            }
        }
    }

    Dialog {
        id: deleteDialog
        onAccepted: {
            archiveBook.document.deletePages(selectedPages)
            reloadModel()
            editActionsItem.selectionModeActive = false
        }
    }
}
