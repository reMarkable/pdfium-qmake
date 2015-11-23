import QtQuick 2.0
import com.magmacompany 1.0

Image {
    id: document

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (file < 5) {
            file++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
        if (file > 1) {
            file--
            event.accepted = true
        }
    }

    DrawingArea {
        currentBrush: DrawingArea.Pen
        anchors.fill: parent
    }

    property int file: 1

    //source: "file:///home/sandsmark/Downloads/pdf-png/lol-" + file + ".png"
    source: "file:///data/pdf/" + file + ".png"

    Rectangle {
        id: nextPageButton
        width: 100
        height: width
        visible: document.file < 5
        border.width: 1
        anchors {
            bottom: top.bottom
            right: document.right
        }
        Text {
            anchors.centerIn: parent
            text: ">"
        }
        MouseArea {
            enabled: nextPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.file++
            }
        }
    }

    Rectangle {
        id: prevPageButton
        width: 100
        height: width
        visible: document.file > 1
        border.width: 1
        anchors {
            bottom: top.bottom
            left: document.left
        }
        Text {
            anchors.centerIn: parent
            text: "<"
        }
        MouseArea {
            enabled: prevPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.file--
            }
        }
    }
}
