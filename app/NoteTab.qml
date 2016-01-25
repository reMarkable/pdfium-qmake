import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    property int iconMargin: 8

    DrawingArea {
        id: drawingArea
        anchors.fill: parent
        currentBrush: DrawingArea.Pen
        document: Collection.getDocument("/data/documents/note")
    }

    Column {
        id: toolBox
        width: 64
        height: 100
        visible: !rootItem.focusMode
        spacing: 5
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: iconMargin
            topMargin: 100
        }

        ToolButton {
            icon: "qrc:/icons/clear page.svg"
            onClicked: drawingArea.clear()
        }

        ToolButton {
            icon: "qrc:/icons/undo.svg"
            onClicked: drawingArea.undo()
        }

        ToolButton {
            icon: "qrc:/icons/redo.svg"
            onClicked: drawingArea.redo()
        }

        ToolButton {
            icon: "qrc:/icons/focus+.svg"
            onClicked: rootItem.focusMode = true
        }
    }
}

