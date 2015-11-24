import QtQuick 2.0
import com.magmacompany 1.0

Item {
    id: document

    onVisibleChanged: {
        if (visible) {
            forceActiveFocus()
        } else {
            rootItem.forceActiveFocus()
        }
    }

    Keys.onRightPressed: {
        if (page < 5) {
            page++
            event.accepted = true
        }
    }

    Keys.onLeftPressed: {
        if (page > 1) {
            page--
            event.accepted = true
        }
    }

    property int page: 1

    Repeater {
        id: pageRepeater
        model: [1,2,3,4,5]
        delegate: Image {
            anchors.fill: document
            visible: document.page === index
            source: "file:///data/pdf/" + index + ".png"


            DrawingArea {
                currentBrush: DrawingArea.Pen
                anchors.fill: parent
            }
        }
    }

    Rectangle {
        id: nextPageButton
        width: 100
        height: width
        visible: document.page < 5
        border.width: 1
        anchors {
            bottom: top.bottom
            right: document.right
        }
        Text {
            anchors.centerIn: parent
            text: "NEXT\nPAGE"
            horizontalAlignment: Text.AlignHCenter
        }
        MouseArea {
            enabled: nextPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.page++
            }
        }
    }

    Rectangle {
        id: prevPageButton
        width: 100
        height: width
        visible: document.page > 1
        border.width: 1
        anchors {
            bottom: top.bottom
            left: document.left
        }
        Text {
            anchors.centerIn: parent
            text: "PREVIOUS\nPAGE"
            horizontalAlignment: Text.AlignHCenter
        }
        MouseArea {
            enabled: prevPageButton.visible
            anchors.fill: parent
            onClicked: {
                document.page--
            }
        }
    }
}
