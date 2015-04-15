import QtQuick 2.1
import QtQuick.Window 2.0

Rectangle {
    id: root
    //color: "lightgray"
    width: 1024
    height: 768

/* XXX for next phase
    VisualItemModel {
        id: itemModel
        Rectangle {
            width: view.width; height: view.height
            Diagram {
                //width:  1024*parent.height/768
                //height: 768*width/1024
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Rectangle {
            width: view.width; height: view.height
            color: "#F0FFF7"
            Text { text: "Policy Rule File Viewer"; font.bold: true; anchors.centerIn: parent }
        }
    }
*/

    Rectangle {
        id: topMenu
        //color: "lightGray"
        width: parent.width
        height: parent.height/12
        Image {
            height: parent.height*2/3
            fillMode: Image.PreserveAspectFit
            source: "images/windriver-logo.png"
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "GENIVI® Audio Manager Monitor"
            anchors.right: parent.right
            font.pointSize: parent.height / 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: parent.width/40

        }
    }
    Diagram {
	anchors.top: topMenu.bottom
	//width:  1024*parent.height/768
	//height: 768*width/1024
	width:  parent.width
	height: parent.height * 11/12
	anchors.horizontalCenter: parent.horizontalCenter
    }

/* XXX for next phase
    ListView {
        id: view
        anchors { fill: parent; bottomMargin: pageIndexer.height ; topMargin: topMenu.height}
        model: itemModel
        preferredHighlightBegin: 0; preferredHighlightEnd: 0
        highlightRangeMode: ListView.StrictlyEnforceRange
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem; flickDeceleration: 2000
    }

    Rectangle {
        id: pageIndexer
        width: parent.width; height: parent.height/25
        //anchors { top: view.bottom; bottom: parent.bottom }
        //color: "gray"
        anchors.bottom: parent.bottom

        Row {
            anchors.centerIn: parent
            spacing: 20

            Repeater {
                model: itemModel.count

                Rectangle {
                    width: 5; height: 5
                    radius: 3
                    color: view.currentIndex == index ? "black" : "darkGray"

                    MouseArea {
                        width: 20; height: 20
                        anchors.centerIn: parent
                        onClicked: view.currentIndex = index
                    }
                }
            }
        }
    }
*/
}
