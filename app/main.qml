import QtQuick 2.3
import QtQuick.Window 2.2
import com.magmacompany 1.0

Window {
    id: window
    visible: true
    height: 1200
    width: 1600
    flags: Qt.Dialog

    property string inactiveColor: "black"
    property string fontColor: "black"
    property string inactiveFontColor: "white"

    Item {
        id: rootItem
        anchors.centerIn: parent
        width: (rotation === 0) ? parent.width : parent.height
        height: (rotation === 0) ? parent.height : parent.width
        rotation: Settings.getValue(Settings.Rotation, 270)

        property bool focusMode: false

        function rotateScreen() {
            if (rotation === 270) {
                rotation = 0
            } else {
                rotation = 270
            }
            Settings.setValue(Settings.Rotation, rotation)
        }

        property bool homeRecentlyClicked: false
        Timer {
            id: homeButtonTimer
            onTriggered: {
                tabBar.setCurrentTab(0)
                rootItem.homeRecentlyClicked = false
            }
            interval: 200
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Home) {
                if (rootItem.focusMode) {
                    rootItem.focusMode = false

                } else if(homeRecentlyClicked) {
                    homeButtonTimer.stop()
                    mainScreen.newNoteClicked()
                    homeRecentlyClicked = false
                } else {
                    homeRecentlyClicked = true
                    homeButtonTimer.restart()
                }
                event.accepted = true
                return
            } else if (event.key === Qt.Key_PowerOff) {
                console.log("Poweroff requested")
                shutdownDialog.visible = true
                event.accepted = true
                return
            }
        }

        Component.onCompleted: forceActiveFocus()

        TabBar {
            id: tabBar
            height: rootItem.focusMode ? 0 : 75
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            function openDocument(path, pageToShow) {
                var index = getTabIndex(path)
                if (index !== -1) {
                    showTab(path, pageToShow)
                    return objectList[index].document
                }

                if (objectList.length > 4) {
                    tooMainTabsDialog.visible = true
                    return null
                }

                var document = Collection.getDocument(path)
                if (!document) {
                    console.log("Unable to get document")
                    return
                }

                var createdObject = noteComponent.createObject(viewRoot, {"document": document})

                // Reset list so that the repeater gets updated
                var objects = objectList
                objects.push(createdObject)
                objectList = objects

                showTab(path, pageToShow)

                return createdObject.document
            }

            function closeDocument(documentPath) {
                var index = getTabIndex(documentPath)
                if (index === -1) {
                    return
                }
                closePage(index)
            }


            function openArchive() {
                var path = "archive"

                if (tabBar.isTabOpen(path)) {
                    tabBar.showTab(path, -1)
                    return
                }

                console.time("archiveclicked")

                if (objectList.length > 4) {
                    tooMainTabsDialog.visible = true
                    return
                }

                var createdObject = archiveComponent.createObject(viewRoot)
                var objects = tabBar.objectList
                objects.splice(0, 0, createdObject)
                tabBar.objectList = objects
                tabBar.setCurrentTab(1)
                console.timeEnd("archiveclicked")
            }
        }

        Component {
            id: noteComponent

            NoteTab {
                anchors.fill: parent
            }
        }

        Component {
            id: archiveComponent

            ArchiveView {
                id: archiveView
                anchors.fill: parent

                onOpenBook: {
                    tabBar.openDocument(path, -1)
                }

                onOpenBookAt: {
                    tabBar.openDocument(path, page)
                }
            }
        }

        Item {
            id: viewRoot

            anchors {
                top: tabBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            MainScreen {
                id: mainScreen
                anchors.fill: parent

                onNewBookClicked: {
                    if (tabBar.objectList.length > 4) {
                        return
                    }

                    var path = Collection.createDocument(type)
                    tabBar.openDocument(path, -1)
                }

                onNewPageClicked: {
                    var path = Collection.defaultDocumentPath(type)

                    var document = tabBar.openDocument(path, -1)
                    document.addPage(type)
                }

                onArchiveClicked: {
                    tabBar.openArchive()
                }

                onOpenBook: {
                    tabBar.openDocument(path, -1)
                }
            }
        }

        Dialog {
            id: tooMainTabsDialog
            message: "Too many tabs open"
            anchors.fill: parent
        }

        Rectangle {
            id: debugWindow
            visible: false
            anchors {
                right: parent.right
                bottom: parent.bottom
            }
            width: 150
            height: 20
            onVisibleChanged: {
                if (visible) {
                    SystemMonitor.start()
                } else {
                    SystemMonitor.stop()
                }
            }

            Text {
                anchors.fill: parent
                text: SystemMonitor.memoryUsed + " MB used"
            }
        }

        Rectangle {
            id: grayOverlay
            anchors.fill: parent
            color: "#7f000000"
            visible: shutdownDialog.visible
        }

        Rectangle {
            id: shutdownDialog
            anchors.centerIn: parent
            width: 750
            height: 400
            border.width: 5
            radius: 10
            color: "white"
            visible: false

            Image {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    topMargin: parent.height / 4 - height / 2
                }
                source: "qrc:/icons/Power-off.svg"
                height: 50
                width: height
            }

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.verticalCenter
                }

                text: "Are you sure you want to shut down?"
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
                        enabled: shutdownDialog.visible
                        anchors.fill: parent
                        onClicked: Qt.quit()
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
                        enabled: shutdownDialog.visible
                        anchors.fill: parent
                        onClicked: shutdownDialog.visible = false
                    }
                }
            }
        }
    }
}
