import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: archiveBook
    
    visible: archiveView.currentBook != ""
    property QtObject document

    property bool selectionModeActive: false
    onSelectionModeActiveChanged: {
        selectedPages = []
    }

    property var selectedPages: []

    property int currentPage: 0
    property int pageCount: 0

    property int maxDisplayItems: 9

    onVisibleChanged: {
        if (!visible) {
            document = null
            pageCount = 0
            pageRepeater.model = 0
        } else {
            document = Collection.getDocument(archiveView.currentBook)
            document.preload()
            pageCount = Math.ceil(document.pageCount / (thumbnailGrid.rows * thumbnailGrid.columns))
            currentPage = 0
            pageRepeater.model = Math.min(document.pageCount - currentPage * maxDisplayItems, maxDisplayItems)
        }
    }

    onCurrentPageChanged: {
        if (!document) {
            return
        }
        var visiblePages = Math.min(document.pageCount - currentPage * maxDisplayItems, maxDisplayItems)
        if (visiblePages != pageRepeater.count) {
            pageRepeater.model = visiblePages
        }
    }

    function deletePages() {
        if (selectedPages.length === 0) {
            return
        }

        if (selectedPages.length > 5) {
            deleteDialog.show("Are you sure you want to delete these pages?")
        } else if (selectedPages.length > 1) {
            var pageNumbers = []
            for (var pageNumber in selectedPages) {
                pageNumbers.push(selectedPages[pageNumber] + 1)
            }

            deleteDialog.show("Are you sure you want to delete pages " + pageNumbers.join(", ") + "?")
        } else {
            deleteDialog.show("Are you sure you want to delete page " + selectedPages[0] + "?")
        }
    }

    function deletePage(index) {
        console.log(index)
        if (index < 0 || index > archiveBook.document.pageCount) {
            return
        }

        selectedPages = [index]
        deleteDialog.show("Are you sure you want to delete page " + (index + 1) + "?")
    }
    
    Grid {
        id: thumbnailGrid
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
                height: 400
                property int pageNumber: index + archiveBook.currentPage * archiveBook.maxDisplayItems
                property bool selected: (archiveBook.selectedPages.indexOf(pageNumber) !== -1)

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
                        
                        source: "file://" + archiveView.currentBook + "-" + bookItem.pageNumber + ".thumbnail.jpg"
                        sourceSize.width: width
                        sourceSize.height: height
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "#7f000000"
                            visible: archiveBook.selectionModeActive && !bookItem.selected
                        }
                        
                        Image {
                            anchors.centerIn: parent
                            width: parent.width / 2
                            height: width
                            visible: archiveBook.selectionModeActive
                            source: bookItem.selected ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white-2.svg"
                            sourceSize.width: width
                            sourceSize.height: height
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!archiveBook.selectionModeActive) {
                                    archiveView.openBookAt(archiveView.currentBook, bookItem.pageNumber)
                                    return
                                }

                                var selectedPages = archiveBook.selectedPages
                                if (bookItem.selected) {
                                    selectedPages.splice(selectedPages.indexOf(bookItem.pageNumber), 1)
                                } else {
                                    selectedPages.push(bookItem.pageNumber)
                                }
                                console.log(selectedPages)
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
                        icon: "qrc:///icons/xoom+_white.svg"
                        visible: !archiveBook.selectionModeActive

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
                        visible: !archiveBook.selectionModeActive
                        
                        icon: "qrc:///icons/Delete_white.svg"
                        onClicked: archiveBook.deletePage(pageNumber)
                    }
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

        visible: archiveBook.currentPage < pageRowRepeater.count - 1
        icon: "qrc:///icons/forward_white.svg"
        color: "#666"
        onClicked: archiveBook.currentPage++
    }

    ArchiveButton {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: 50
        }
        visible: archiveBook.currentPage > 0
        icon: "qrc:///icons/back_white.svg"
        color: "#666"
        onClicked: archiveBook.currentPage--
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
            model: archiveBook.pageCount
            delegate: Rectangle {
                width: 40
                height: width
                radius: 2
                color: archiveBook.currentPage === index ? "black" : "gray"

                MouseArea {
                    anchors.fill: parent
                    onClicked: archiveBook.currentPage = index
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
            sourceSize.width: width
            sourceSize.height: height

            ArchiveButton {
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 50
                }
                icon: "qrc:///icons/forward_white.svg"
                visible: (archiveBook.document !== null) ? (previewBackground.index < archiveBook.document.pageCount - 1) : false
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
                onClicked: archiveView.openBookAt(archiveView.currentBook, previewBackground.index)
            }
        }
    }

    Dialog {
        id: deleteDialog
        onAccepted: {
            for (var i=0; i<archiveBook.selectedPages.length; i++) {
                console.log(archiveBook.selectedPages[i])
                //Collection.deleteDocument(archiveMain.selectedBooks[i])
            }
            archiveBook.selectionModeActive = false
        }
    }
}
