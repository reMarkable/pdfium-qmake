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
        rotation: Settings.getValue(Settings.Rotation, 0)

        property bool focusMode: false

        function rotateScreen() {
            if (rotation === 270) {
                rotation = 0
            } else {
                rotation = 270
            }
            Settings.setValue(Settings.Rotation, rotation)
        }

        function openDocument(path) {
            var name = Collection.title(path)
            var index = tabBar.tabModel.indexOf(name)

            if (index === -1) {
                var newIndex = tabBar.tabModel.length + 1
                var createdObject = documentComponent.createObject(rootItem, {"tabIndex": newIndex})
                var tabModel = tabBar.tabModel
                tabModel.push(name)
                tabBar.tabModel = tabModel
                tabBar.currentTab = newIndex
                tabBar.objectList.push(createdObject)
                createdObject.documentPath = path
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
                    rootItem.openDocument(path)
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
                    var createdObject = archiveComponent.createObject(rootItem, {"tabIndex": newIndex })
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
                rootItem.openDocument(path)
            }
        }
    }
}
