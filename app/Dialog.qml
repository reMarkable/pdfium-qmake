import QtQuick 2.0
import com.magmacompany 1.0

Rectangle {
    id: dialog
    width: rootItem.width
    height: rootItem.height
    x: - (viewRoot.x + parent.x)
    y: - (viewRoot.y + parent.y)
    visible: false

    function show(question) {
        dialog.question = question
        visible = true
    }

    property string message: ""
    property string question: ""
    signal accepted
    
    color: "#7f000000"

    function dismiss() {
        dialog.message = ""
        dialog.question = ""
        dialog.visible = false
    }

    MouseArea {
        anchors.fill: parent
        onClicked: dialog.dismiss()
    }
    
    Rectangle {
        id: dialogContent
        anchors.centerIn: parent
        width: 750
        height: 400
        border.width: 5
        radius: 10
        color: "white"
        
        Text {
            anchors {
                left: parent.left
                right: parent.right
                margins: 75
                bottom: parent.verticalCenter
            }
            
            text: dialog.question !== "" ? dialog.question : (dialog.message !== "" ? dialog.message : "")
            font.pointSize: 18
            wrapMode: Text.Wrap
        }
        
        Item {
            id: yesButton
            visible: dialog.question !== ""
            anchors {
                right: parent.horizontalCenter
                left: parent.left
                bottom: parent.bottom
                top: parent.verticalCenter
            }
            
            Image {
                anchors.centerIn: parent
                source: "qrc:/icons/yes.svg"
                height: 100
                width: height
                sourceSize.width: width
                sourceSize.height: height
                MouseArea {
                    enabled: dialogContent.visible
                    anchors.fill: parent
                    onClicked: {
                        dialog.accepted()
                        dialog.visible = false
                    }
                }
            }
        }
        
        Item {
            id: noButton
            visible: dialog.question !== ""
            anchors {
                left: parent.horizontalCenter
                right: parent.right
                bottom: parent.bottom
                top: parent.verticalCenter
            }
            
            Image {
                anchors.centerIn: parent
                source: "qrc:/icons/no.svg"
                height: 100
                width: height
                sourceSize.width: width
                sourceSize.height: height
                MouseArea {
                    enabled: dialogContent.visible
                    anchors.fill: parent
                    onClicked: {
                        dialog.dismiss()
                    }
                }
            }
        }

        Item {
            id: okButton
            visible: dialog.message !== ""
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                top: parent.verticalCenter
            }

            Image {
                anchors.centerIn: parent
                source: "qrc:/icons/yes.svg"
                height: 100
                width: height
                sourceSize.width: width
                sourceSize.height: height
                MouseArea {
                    enabled: dialogContent.visible
                    anchors.fill: parent
                    onClicked: {
                        dialog.accepted()
                        dialog.visible = false
                    }
                }
            }
        }
    }
}
