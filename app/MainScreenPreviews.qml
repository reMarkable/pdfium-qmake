import QtQuick 2.5
import com.magmacompany 1.0

Item {
    id: mainScreenPreviews

    property alias model: frequentlyUsedRepeater.model
    property alias rows: previewsGrid.rows
    property alias columns: previewsGrid.columns
    property alias title: headerText.text

    signal openClicked()

    height: previewsGrid.anchors.bottomMargin + rows * hugeIconSize + headerText.height + headerText.anchors.topMargin + header.height + 50

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
            rightMargin: 50
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
            topMargin: 0
            bottom: parent.bottom
            bottomMargin: 30
        }
        horizontalItemAlignment: Grid.AlignHCenter
        verticalItemAlignment: Grid.AlignTop
        columnSpacing: (width / columns) - bigIconSize
        //height: bigIconSize * rows

        Repeater {
            id: frequentlyUsedRepeater
            
            Rectangle {
                width: bigIconSize
                height: previewsGrid.height / previewsGrid.rows
                border.width: 0

                Image {
                    anchors.centerIn: parent
                    height: bigIconSize - 2
                    width: height
                    source: Collection.thumbnailPath(modelData)
                    fillMode: Image.PreserveAspectCrop
                    
                    Image {
                        anchors.fill: parent
                        source: "qrc:/icons/clear page-thumb.svg"
                    }
                }
                
                Text {
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: Collection.title(modelData)
                    color: window.fontColor
                    font.pointSize: 18
                    elide: Text.ElideLeft
                    width: parent.width
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mainScreen.openBook(modelData)
                    }
                }
            }
        }
    }
}
