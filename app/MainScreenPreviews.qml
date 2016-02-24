import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: mainScreenPreviews

    property alias model: frequentlyUsedRepeater.model
    property alias rows: previewsGrid.rows
    property alias columns: previewsGrid.columns
    property alias title: headerText.text

    property int thumbnailHeight: 300
    property int thumbnailWidth: thumbnailHeight * 3 / 4

    signal openClicked()

    height: previewsGrid.height + headerText.height + header.height

    Text {
        id: headerText
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
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
            left: parent.left
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
        }
        width: 150

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
            right: openButton.left
            top: header.bottom
            topMargin: 20
        }
        height: rows * mainScreenPreviews.thumbnailHeight + 75

        Repeater {
            id: frequentlyUsedRepeater

            Item {
                height: previewsGrid.height / previewsGrid.rows
                width: previewsGrid.width / previewsGrid.columns
                BookThumbnail {
                    anchors.centerIn: parent
                    width: mainScreenPreviews.thumbnailWidth
                    height: mainScreenPreviews.thumbnailHeight
                    onClicked: mainScreen.openBook(modelData)
                }
            }
        }
    }

}
