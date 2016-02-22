import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: mainScreenPreviews

    property alias model: frequentlyUsedRepeater.model
    property alias rows: previewsGrid.rows
    property alias columns: previewsGrid.columns
    property alias title: headerText.text

    property int thumbnailHeight: 300
    property int thumbnailWidth: 220

    signal openClicked()

    height: previewsGrid.anchors.bottomMargin + previewsGrid.anchors.topMargin + previewsGrid.rowSpacing + rows * thumbnailHeight + headerText.height + headerText.anchors.topMargin + header.height

    Text {
        id: headerText
        anchors {
            left: parent.left
            leftMargin: 20
            right: parent.right
            top: parent.top
            topMargin: 0
        }
        color: window.fontColor
        text: mainScreenPreviews.title
        font.pointSize: 18
        font.bold: true
    }

    Rectangle {
        id: header
        anchors {
            top: headerText.bottom
            right: parent.right
            rightMargin: 30
            left: parent.left
            leftMargin: 30
        }

        height: 3
        color: "gray"
    }

    Item {
        id: openButton
        anchors {
            top: header.bottom
            bottom: parent.bottom
            right: parent.right
            rightMargin: 45
        }
        width: mediumIconSize

        Image {
            anchors {
                centerIn: parent
            }

            height: mediumIconSize
            width: height
            source: "qrc:/icons/forward.svg"
            smooth: true

            MouseArea {
                anchors.fill: parent
                onClicked: mainScreenPreviews.openClicked()
            }
        }
    }

    Grid {
        id: previewsGrid
        
        anchors {
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 100
            top: header.bottom
            topMargin: 20
            bottom: parent.bottom
            bottomMargin: 0
        }
        horizontalItemAlignment: Grid.AlignHCenter
        verticalItemAlignment: Grid.AlignVCenter
        columnSpacing: ((width - openButton.width) / columns) - bigIconSize
        rowSpacing: 20

        Repeater {
            id: frequentlyUsedRepeater

            BookThumbnail {
                width: mainScreenPreviews.thumbnailWidth
                height: mainScreenPreviews.thumbnailHeight
            }
        }
    }
}
