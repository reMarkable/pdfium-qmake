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

        function endsWith(string, suffix) {
            return string.indexOf(suffix, string.length - suffix.length) !== -1
        }

        function openDocument(path, pageToOpen) {
            var name = Collection.title(path)
            var index = tabBar.tabModel.indexOf(name)

            if (index === -1) {
                if (tabBar.tabModel.length > 4) {
                    tooMainTabsDialog.visible = true
                    return
                }

                var newIndex = tabBar.tabModel.length + 1
                var createdObject;
                if (endsWith(path, ".pdf")) {
                    createdObject = documentComponent.createObject(viewRoot, {"tabIndex": newIndex})
                    createdObject.documentPath = path
                } else {
                    createdObject = noteComponent.createObject(viewRoot, {"tabIndex": newIndex})
                    createdObject.documentPath = path

                    createdObject.document = Collection.getDocument(path)
                }
                if (pageToOpen !== -1) {
                    console.log(pageToOpen)
                    createdObject.document.currentIndex = pageToOpen
                }

                var objectList = tabBar.objectList
                objectList.push(createdObject)
                tabBar.objectList = objectList
                tabBar.setCurrentTab(newIndex)
            } else {
                tabBar.setCurrentTab(index + 1)
                if (pageToOpen !== -1) {
                    tabBar.objectList[index].document.currentIndex = pageToOpen
                }
            }
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

            tabModel: []
        }

        Component {
            id: documentComponent

            DocumentTab {
                anchors.fill: parent

                property int tabIndex
            }
        }

        Component {
            id: noteComponent

            NoteTab {
                anchors.fill: parent
                property int tabIndex
            }
        }

        Component {
            id: archiveComponent

            ArchiveView {
                id: archiveView
                anchors.fill: parent
                property int tabIndex

                onOpenBook: {
                    rootItem.openDocument(path, -1)
                }
                onOpenBookAt: {
                    rootItem.openDocument(path, page)
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
                    if (tabBar.tabModel.length > 4) {
                        return
                    }

                    var newIndex = tabBar.tabModel.length + 1
                    var createdObject = noteComponent.createObject(viewRoot, {"tabIndex": newIndex})
                    var path = Collection.createDocument(type)
                    createdObject.document = Collection.getDocument(path)
                    var objectList = tabBar.objectList
                    objectList.push(createdObject)
                    tabBar.objectList = objectList
                    tabBar.setCurrentTab(newIndex)
                }

                onNewPageClicked: {
                    var name
                    if (type === "Sketch") {
                        name = "Default sketchbook"
                    } else {
                        name = "Default notebook"
                    }
                    var index = tabBar.tabModel.indexOf(name)

                    if (index === -1) {
                        if (tabBar.tabModel.length > 4) {
                            tooMainTabsDialog.visible = true
                            return
                        }

                        var newIndex = tabBar.tabModel.length + 1
                        var createdObject = noteComponent.createObject(viewRoot, {"tabIndex": newIndex})

                        createdObject.document = Collection.getDefaultDocument(type)
                        createdObject.document.addPage(type)

                        var objectList = tabBar.objectList
                        objectList.push(createdObject)
                        tabBar.objectList = objectList
                        tabBar.setCurrentTab(newIndex)
                    } else {
                        tabBar.setCurrentTab(index + 1)
                        tabBar.objectList[index].document.addPage(type)
                    }
                }

                onArchiveClicked: {
                    console.time("archiveclicked")
                    var index = tabBar.tabModel.indexOf("Archive")

                    if (index === -1) {
                        if (tabBar.tabModel.length > 4) {
                            tooMainTabsDialog.visible = true
                            return
                        }

                        var createdObject = archiveComponent.createObject(viewRoot, {"tabIndex": 1 })
                        var objectList = tabBar.objectList
                        objectList.splice(0, 0, createdObject)
                        tabBar.objectList = objectList
                        tabBar.setCurrentTab(1)
                    } else {
                        tabBar.setCurrentTab(index + 1)
                    }
                    console.timeEnd("archiveclicked")
                }

                onOpenBook: {
                    rootItem.openDocument(path, -1)
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
