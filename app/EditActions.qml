import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: editActionsItem
    
    height: 75

    property bool selectionModeActive: false
    signal deleteItems()
    property int maxDisplayItemCount: 9
    
    Rectangle {
        id: modeSelect
        anchors {
            top: parent.top
            right: parent.right
        }
        
        width: 150
        height: 75
        color: "#666"
        radius: 5
        
        ArchiveButton {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            color: "#666"
            icon: "qrc:///icons/Grid 9.svg"

            onClicked: editActionsItem.maxDisplayItemCount = 9
        }
        
        ArchiveButton {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            color: "#666"
            icon: "qrc:///icons/Grid 36_white.svg"
            onClicked: editActionsItem.maxDisplayItemCount = 36
        }
    }
    
    ArchiveButton {
        id: editButton
        anchors {
            top: parent.top
            right: modeSelect.left
            rightMargin: 10
        }
        color: "#666"
        icon: editActionsRow.visible ? "qrc:///icons/yes.svg" : "qrc:///icons/yes_white.svg"
        onClicked: {
            editActionsItem.selectionModeActive = !editActionsItem.selectionModeActive
        }
    }
    
    Rectangle {
        anchors.fill: editActionsRow
        color: "#666"
        visible: editActionsRow.visible
        radius: 5
    }
    
    Row {
        id: editActionsRow
        anchors {
            verticalCenter: editButton.verticalCenter
            right: editButton.left
            rightMargin: 10
        }
        visible: editActionsItem.selectionModeActive
        
        ArchiveButton {
            color: "#666"
            icon: "qrc:///icons/Delete_white.svg"
            onClicked: {
                editActionsItem.deleteItems()
            }
        }
    }
}
