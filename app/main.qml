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

        Keys.onPressed: {
            if (event.key === Qt.Key_Home) {
                if (window.index === 0) {
                    window.index = 2
                } else {
                    window.index = 0
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

        TabBar {
            id: tabBar
            height: 60
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
            visible: (tabBar.currentTab === 0)
            anchors {
                top: tabBar.bottom
                right: parent.right
                left: parent.left
                bottom: parent.bottom
            }

            onNewNoteClicked: {
            }

            onNewSketchClicked: {
                window.index = 3
            }

            onArchiveClicked: {
                if (archiveIndex === -1) {
                    tabBar.currentTab = 1
                    return
                }
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
                            name: "PHOTOS"
                            files: [
                                ListElement { path: "file:///data/testillustr.jpg" },
                                ListElement { path: "file:///data/typoskjerm.png" }
                            ]
                        }
                    ]
                }
            }
        }
    }
}
