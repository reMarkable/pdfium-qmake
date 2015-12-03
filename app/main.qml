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

        Keys.onPressed: {
            if (event.key === Qt.Key_Home) {
                if (rootItem.focusMode) {
                    rootItem.focusMode = false
                } else {
                    tabBar.currentTab = 0
                    event.accepted = true
                }
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

            tabModel: ["ARCHIVE"]
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
            archiveModel: archiveView.folderModel.get(0).files
            visible: (tabBar.currentTab === 0)
            anchors {
                top: tabBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
                leftMargin: 10
                rightMargin: 10
                bottomMargin: 10
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
                tabBar.currentTab = 1
            }

            onOpenBook: {
                rootItem.openDocument(name, files)
            }
        }

        ArchiveView {
            id: archiveView
            visible: (tabBar.currentTab === 1)
            anchors {
                top: tabBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            onOpenBook: {
                rootItem.openDocument(name, files)
            }

            folderModel: ListModel {
                ListElement {
                    name: "DROPBOX"
                    files: [
                        ListElement {
                            name: "DESTIN"
                            files: [
                                ListElement { path: "file:///data/pdf/1.png" },
                                ListElement { path: "file:///data/pdf/2.png" },
                                ListElement { path: "file:///data/pdf/3.png" },
                                ListElement { path: "file:///data/pdf/4.png" },
                                ListElement { path: "file:///data/pdf/5.png" }
                            ]
                        },
                        ListElement {
                            name: "DIJKSTRA"
                            files: [
                                ListElement { path: "file:///data/dijkstra-01.png" },
                                ListElement { path: "file:///data/dijkstra-02.png" },
                                ListElement { path: "file:///data/dijkstra-03.png" },
                                ListElement { path: "file:///data/dijkstra-04.png" },
                                ListElement { path: "file:///data/dijkstra-05.png" },
                                ListElement { path: "file:///data/dijkstra-06.png" },
                                ListElement { path: "file:///data/dijkstra-07.png" },
                                ListElement { path: "file:///data/dijkstra-08.png" },
                                ListElement { path: "file:///data/dijkstra-09.png" },
                                ListElement { path: "file:///data/dijkstra-10.png" },
                                ListElement { path: "file:///data/dijkstra-11.png" },
                                ListElement { path: "file:///data/dijkstra-12.png" },
                                ListElement { path: "file:///data/dijkstra-13.png" },
                                ListElement { path: "file:///data/dijkstra-14.png" },
                                ListElement { path: "file:///data/dijkstra-15.png" },
                                ListElement { path: "file:///data/dijkstra-16.png" },
                                ListElement { path: "file:///data/dijkstra-17.png" },
                                ListElement { path: "file:///data/dijkstra-18.png" },
                                ListElement { path: "file:///data/dijkstra-19.png" },
                                ListElement { path: "file:///data/dijkstra-20.png" },
                                ListElement { path: "file:///data/dijkstra-21.png" },
                                ListElement { path: "file:///data/dijkstra-22.png" },
                                ListElement { path: "file:///data/dijkstra-23.png" },
                                ListElement { path: "file:///data/dijkstra-24.png" },
                                ListElement { path: "file:///data/dijkstra-25.png" },
                                ListElement { path: "file:///data/dijkstra-26.png" },
                                ListElement { path: "file:///data/dijkstra-27.png" },
                                ListElement { path: "file:///data/dijkstra-28.png" },
                                ListElement { path: "file:///data/dijkstra-29.png" },
                                ListElement { path: "file:///data/dijkstra-30.png" }
                            ]
                        },
                        ListElement {
                            name: "IMAGES"
                            files: [
                                ListElement { path: "file:///data/test.png" },
                                ListElement { path: "file:///data/typoskjerm.png" }
                            ]
                        }
                    ]
                }
            }
        }
    }
}
