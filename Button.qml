/* SPDXLicenseID: MPL-2.0
*
* Copyright (C) 2014, GENIVI Alliance
*
* This file is part of AudioManager Monitor
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License (MPL), v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* For further information see http://www.genivi.org/.
*
* List of changes:
*/

import QtQuick 2.0
import com.windriver.ammonitor 1.0

Item {
    id :button
    anchors.horizontalCenter: parent.horizontalCenter
    width: 100
    height: 100
    property bool pressed: hitbox.pressed && hitbox.containsMouse

    property var amCommandIF
    property string amSource
    property string amSink
    property string mediaRole
    property string audioFilePath
    property int connectionID : 0
    property string iconName
    property string title : "no title"

    Rectangle {
        anchors.fill: parent
        anchors.margins: 4
        color: paplayer.playing?"darkGray":"lightGray"
        radius: 10
        border.width: 1
        border.color: "gray"

        Image {
            id: icon
            height: (pressed||paplayer.playing)?(parent.height/3):(parent.height/2)
            fillMode: Image.PreserveAspectFit
            anchors.horizontalCenter: parent.horizontalCenter
            y: (parent.height - (height + titleText.height)) / 2
            source: "images/icon_"+iconName+".png"
        }
        Image {
            height: parent.height / 5
            fillMode: Image.PreserveAspectFit
            x: parent.width - width - (width/2)
            y: (height/2)
            source: "images/volume-up.png"
            visible: paplayer.playing
        }
        Text {
            id: titleText
            anchors.top: icon.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: title
            font.pixelSize: parent.height / 6
        }
        Text {
            id: descText
            visible: pressed||paplayer.playing
            anchors.top: titleText.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            //text: "Connecting '"+amSource+"' to '"+amSink+"'"
            text: amSource+" => "+amSink
            font.pixelSize: parent.height / 10
        }
        PAPlayer {
            id: paplayer
            role: mediaRole
            file: audioFilePath
            source: amSource
            sink: amSink
            onPlayStateChanged: {
                if(!playing && button.connectionID != 0) {
                    console.log('11 trying to disconnect AM connection: '+button.connectionID)
                    amCommandIF.disconnect(button.connectionID);
                    button.connectionID = 0
                }
            }
        }

        MouseArea {
            id: hitbox
            anchors.fill: parent
            onClicked : {
                if(paplayer.isPlaying()) {
                    console.log('trying to disconnect AM connection: '+button.connectionID)
                    if(button.connectionID != 0) {
                        if(amCommandIF.disconnect(button.connectionID))
                            paplayer.stop()
                        button.connectionID = 0
                    }
                } else {
                    button.connectionID = amCommandIF.connect(amSource, amSink);
                    console.log('AM Connected: '+button.connectionID)
                    if(button.connectionID > 0)
                        paplayer.play()
                }
            }
        }
    }
}

