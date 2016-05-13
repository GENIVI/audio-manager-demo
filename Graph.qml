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
import "code.js" as Code

Rectangle {
    property string title : "No name"
    property string description : "No description"
    property string backgroundColor : "#F0F0F0"
    property string backgroundLineColor : "#aaaaaa"
    property real backgroundLineWidth : 0.1
    property int refreshInterval : 500
    property string graphName : ""
    property int defaultValue : 0
    property int maxDataLength : 100
    property int maxValue : 100
    property int graphLineWidth : 1
    property int type : 0
    property var graph : null
    property var graphInfo : ({})

    Component.onCompleted: {
        graph = new Code.Graph(type);
        refreshTimer.start();
    }

    function getGraphNodeColor(nodeName) {
        return graph.getGraphNodeColor(nodeName);
    }

    function updateData(name, id, value) {
        var index = graph.checkGraphNodeInList(name, id);
        if (index < 0) {
            console.log("Try to add graphNode(name: " + name + ", id: " + id + " )");
            graph.addGraphNode(name, id, maxDataLength, value);
            return;
        }

        graph.updateGraphNode(index, value);
    }

    function removeData(name, id) {
        var index = graph.checkGraphNodeInList(name, id);
        if (index < 0)
            return;

        console.log("Try to remove graphNode(name: " + name + ", id: " + id + " )");
        graph.removeGraphNode(index);
    }

    function startTimer() {
        refreshTimer.start();
    }

    function stopTimer() {
        refreshTimer.stop();
    }

    Text {
        text: title
        font.pixelSize: parent.height / 20
        anchors.bottom: canvas.top
        anchors.left:  canvas.left
    }

    Text {
        text: maxValue
        font.pixelSize: parent.height / 30
        anchors.right: canvas.left
        anchors.rightMargin: 5
        y: parent.height * 0.25
    }
    Text {
        text: maxValue / 2
        font.pixelSize: parent.height / 30
        anchors.right: canvas.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 5
        y: parent.height * 0.5
    }
    Text {
        id: volume0
        text: "0"
        font.pixelSize: parent.height / 30
        anchors.right: canvas.left
        anchors.rightMargin: 5
        y: parent.height * 0.70
    }

    Text {
        text: "Volume"
        font.pixelSize: parent.height / 30
        anchors.right: canvas.left
        anchors.rightMargin: 2
        anchors.top: volume0.bottom
    }

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height
        anchors.fill: parent
        anchors.margins: parent.width / 10
        antialiasing: true

        signal refreshGraphNode();

        Component.onCompleted: {
            graphInfo.width = width + 91;
            graphInfo.height = height - 22;
            graphInfo.maxDataLength = parent.maxDataLength;
            graphInfo.lineWidth = parent.graphLineWidth;
            graphInfo.maxValue = parent.maxValue;
            graphInfo.backgroundColor = parent.backgroundColor;
            graphInfo.backgroundLineColor = parent.backgroundLineColor;
            graphInfo.backgroundLineWidth = parent.backgroundLineWidth;

            graph.loadGraphInfo(graphInfo);
        }

        onRefreshGraphNode: {
            graph.refreshGraphNode();
        }

        onPaint: {
            var context = canvas.getContext("2d");
            context.reset();

            if (graph == null) {
                console.log("Error: Cannot create graph, " + graph.name);
                return;
            }

            graph.drawBackground(context);
            graph.drawGraphNode(context);
        }
    }

    Timer {
        id: refreshTimer
        interval: parent.refreshInterval
        repeat: true
        onTriggered: {
            canvas.refreshGraphNode();
            canvas.requestPaint();
        }
    }

    Text {
        text: "("+description+")"
        font.pixelSize: parent.height / 30
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: parent.height/14
   }
}
