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

        Rectangle {
            id: debugWindow
            visible: false
            anchors {
                right: parent.right
                bottom: parent.bottom
            }
            width: 150
            height: 20

            Text {
                anchors.fill: parent
                text: SystemMonitor.memoryUsed + " MB used"
            }
        }


        Rectangle {
            id: shutdownDialog
            anchors.centerIn: parent
            width: 850
            height: 400
            border.width: 15
            radius: 10
            color: "white"
            //visible: false

            Text {
                anchors.fill: parent
                anchors.margins: 75
                text: "Are you sure you want to shut down?"
                font.pointSize: 18
                wrapMode: Text.Wrap
            }

            Rectangle {
                anchors {
                    right: parent.horizontalCenter
                    left: parent.left
                    bottom: parent.bottom
                    top: parent.verticalCenter
                    margins: 35
                }

                radius: height / 2
                border.width: 5

                Text {
                    anchors.centerIn: parent
                    text: "Yes"
                    font.pointSize: 18
                }

                MouseArea {
                    enabled: shutdownDialog.visible
                    anchors.fill: parent
                    onClicked: Qt.quit()
                }
            }

            Rectangle {
                anchors {
                    left: parent.horizontalCenter
                    right: parent.right
                    bottom: parent.bottom
                    top: parent.verticalCenter
                    margins: 35
                }

                radius: height / 2
                border.width: 5

                Text {
                    anchors.centerIn: parent
                    text: "No"
                    font.pointSize: 18
                }

                MouseArea {
                    enabled: shutdownDialog.visible
                    anchors.fill: parent
                    onClicked: shutdownDialog.visible = false
                }
            }
        }
    }
}
