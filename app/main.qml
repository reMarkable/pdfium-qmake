import QtQuick 2.3
import QtQuick.Window 2.2

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
        width: parent.height
        height: parent.width
        rotation: 270

        property bool focusMode: false

        function rotateScreen() {
            if (rotation === 270) {
                rotation = 0
                width = parent.width
                height = parent.height
            } else {
                rotation = 270
                width = parent.height
                height = parent.width
            }
        }

        function openDocument(name, files) {
            var index = tabBar.tabModel.indexOf(name)

            if (index === -1) {
                var newIndex = tabBar.tabModel.length + 1
                var createdObject = documentComponent.createObject(rootItem, {"tabIndex": newIndex, "pageModel": files})
                var tabModel = tabBar.tabModel
                tabModel.push(name)
                tabBar.tabModel = tabModel
                tabBar.currentTab = newIndex
                tabBar.objectList.push(createdObject)
            } else {
                tabBar.currentTab = index + 1
            }
        }

        property bool homeRecentlyClicked: false
        Timer {
            id: homeButtonTimer
            onTriggered: {
                tabBar.currentTab = 0
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
                event.accepted = true
                return
            }
            console.log("Key pressed: " + event.key)
        }

        Component.onCompleted: forceActiveFocus()

        TabBar {
            id: tabBar
            height: rootItem.focusMode ? 0 : 60
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
                visible: tabBar.currentTab === tabIndex
                anchors {
                    top: tabBar.bottom
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }

                property int tabIndex
            }
        }

        Component {
            id: noteComponent

            NoteTab {
                visible: tabBar.currentTab === tabIndex
                anchors {
                    top: tabBar.bottom
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }

                property int tabIndex
            }
        }

        Component {
            id: archiveComponent

            ArchiveView {
                id: archiveView
                visible: (tabBar.currentTab === tabIndex)
                anchors {
                    top: tabBar.bottom
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }
                property int tabIndex

                onOpenBook: {
                    rootItem.openDocument(name, files)
                }
            }
        }

        Component {
            id: sketchComponent

            SketchTab {
                visible: tabBar.currentTab === tabIndex
                anchors {
                    top: tabBar.bottom
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }

                property int tabIndex
            }
        }

        MainScreen {
            id: mainScreen
            archiveModel: exampleDocumentModel.get(0).files
            visible: (tabBar.currentTab === 0)
            anchors {
                top: tabBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            onNewNoteClicked: {
                var index = tabBar.tabModel.indexOf("NOTE")

                if (index === -1) {
                    var newIndex = tabBar.tabModel.length + 1
                    var createdObject = noteComponent.createObject(rootItem, {"tabIndex": newIndex})
                    var tabModel = tabBar.tabModel
                    tabModel.push("NOTE")
                    tabBar.tabModel = tabModel
                    tabBar.currentTab = newIndex
                    tabBar.objectList.push(createdObject)
                } else {
                    tabBar.currentTab = index + 1
                }
            }

            onNewSketchClicked: {
                var index = tabBar.tabModel.indexOf("SKETCH")

                if (index === -1) {
                    var newIndex = tabBar.tabModel.length + 1
                    var createdObject = sketchComponent.createObject(rootItem, {"tabIndex": newIndex})
                    var tabModel = tabBar.tabModel
                    tabModel.push("SKETCH")
                    tabBar.tabModel = tabModel
                    tabBar.currentTab = newIndex
                    tabBar.objectList.push(createdObject)
                } else {
                    tabBar.currentTab = index + 1
                }
            }

            onArchiveClicked: {
                var index = tabBar.tabModel.indexOf("ARCHIVE")

                if (index === -1) {
                    var newIndex = tabBar.tabModel.length + 1
                    var createdObject = archiveComponent.createObject(rootItem, {"tabIndex": newIndex, "folderModel": exampleDocumentModel })
                    var tabModel = tabBar.tabModel
                    tabModel.push("ARCHIVE")
                    tabBar.tabModel = tabModel
                    tabBar.currentTab = newIndex
                    tabBar.objectList.push(createdObject)
                } else {
                    tabBar.currentTab = index + 1
                }
            }

            onOpenBook: {
                rootItem.openDocument(name, files)
            }
        }

        ListModel {
            id: exampleDocumentModel
            ListElement {
                name: "DROPBOX"
                files: [
                    ListElement {
                        name: "DESTIN.PDF"
                        description: "Academic paper about machine learning"
                        files: [
                            ListElement { path: "file:///data/pdf/1.png" },
                            ListElement { path: "file:///data/pdf/2.png" },
                            ListElement { path: "file:///data/pdf/3.png" },
                            ListElement { path: "file:///data/pdf/4.png" },
                            ListElement { path: "file:///data/pdf/5.png" }
                        ]
                    },
                    ListElement {
                        name: "JANTU.DOC"
                        description: "Martin's master thesis"
                        files: [
                            ListElement { path: "file:///data/master-17.png" },
                            ListElement { path: "file:///data/master-18.png" },
                            ListElement { path: "file:///data/master-19.png" },
                            ListElement { path: "file:///data/master-20.png" },
                            ListElement { path: "file:///data/master-21.png" },
                            ListElement { path: "file:///data/master-22.png" },
                            ListElement { path: "file:///data/master-23.png" },
                            ListElement { path: "file:///data/master-24.png" },
                            ListElement { path: "file:///data/master-25.png" },
                            ListElement { path: "file:///data/master-26.png" },
                            ListElement { path: "file:///data/master-27.png" }
                        ]
                    },
                    ListElement {
                        name: "IMAGES.ZIP"
                        description: "Some sample images"
                        files: [
                            ListElement { path: "file:///data/diamondsutra.png" },
                            ListElement { path: "file:///data/gutenbergbible-tl.png" },
                            ListElement { path: "file:///data/test.png" },
                            ListElement { path: "file:///data/typoskjerm.png" }
                        ]
                    }
                ]
            }
        }
    }
}
