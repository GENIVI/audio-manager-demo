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

    Component.onCompleted: {
        Code.addGraphDataset(graphName,refreshInterval);
        refreshTimer.start();
    }

    function updateData(name, id, value) {
        Code.updateGraphNode(graphName, name, id, maxDataLength, value);
    }

    function removeData(name, id) {
        Code.removeGraphNode(graphName, name, id);
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
        anchors.bottom: background.top
        anchors.left: background.left
    }

    Text {
        text: maxValue
        font.pixelSize: parent.height / 30
        anchors.right: background.left
        anchors.rightMargin: 5
        y: parent.height * 0.25
    }
    Text {
        text: maxValue / 2
        font.pixelSize: parent.height / 30
        anchors.right: background.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 5
        y: parent.height * 0.5
    }
    Text {
        id: volume0
        text: "0"
        font.pixelSize: parent.height / 30
        anchors.right: background.left
        anchors.rightMargin: 5
        y: parent.height * 0.70
    }

    Text {
        text: "Volume"
        font.pixelSize: parent.height / 30
        anchors.right: background.left
        anchors.rightMargin: 2
        anchors.top: volume0.bottom
    }

    Canvas {
        id: background
        width: parent.width
        height: parent.height
        anchors.fill: parent
        anchors.margins: parent.width / 10
        antialiasing: true

        function drawBackground() {
            // Get Drawing Context
            var context = getContext('2d');
	    context.clearRect(0, 0, width, height);
            var bgColor = parent.backgroundColor;
            var bgLineColor = parent.backgroundLineColor;
            var bgLineWidth = parent.backgroundLineWidth;

            // Fill background color
            context.save();
            /*
            context.beginPath();
            context.fillStyle = bgColor;
            context.fillRect(0,0,width, height);
            context.closePath();
            context.fill();
            */

            // Draw Guide Line

            context.translate(0.8,0.8);
            var horizontalWidth = width / 10;
            var verticalWidth = height / 10;

            for(var i = 0; i < 10; i++) {
                if(i==0)
                    continue
                // Draw Horizontal Line
                context.beginPath();
                context.lineWidth = bgLineWidth;
                context.strokeStyle = bgLineColor;
                context.moveTo(0, verticalWidth * i);
                context.lineTo(width-1, verticalWidth * i);
                context.closePath();
                context.stroke();

                // Draw Vertical Line
                context.moveTo(horizontalWidth * i, 0);
                context.lineTo(horizontalWidth * i, height);

                // Stroke!!
                context.stroke();
            }

            // Draw Right, Bottom border line

            context.translate(-0.8, -0.8);
            var w = width -1;
            var h = height -1;
            context.beginPath();
            context.strokeStyle = "black";
            context.lineWidth = 1;
            context.moveTo(0,0);
            context.lineTo(0, height);
            context.moveTo(0, verticalWidth * 8);
            context.lineTo(width, verticalWidth * 8);
            context.closePath();
            context.stroke();

            context.restore();

        }

        function drawGraphNode() {
            var context = getContext('2d');
            var graphWidth = width-2;
            var graphHeight = height * (0.6);
            var nodeList = Code.takeGraphDataset(graphName);
            var lineColor = "black";
            var dataDrawingWidth = graphWidth / maxDataLength;

            if (nodeList.length == 0) {
                return;
            }

            context.lineWidth = graphLineWidth;

            for (var i = 0; i < nodeList.length; i++) {
                var node = nodeList[i];
                var startX = width-1;
                var startY = height-1;
                var endX = 0;
                var endY = 0;

                context.beginPath();
                lineColor = node.color;
                context.strokeStyle = lineColor;
                if (node.removed)
                    startX = dataDrawingWidth * node.data.length;

                for (var j = node.data.length-1; j > 0; j--) {
                    var value = node.data[j];
                    var adjustY = 0;
                    if (j == node.data.length-1) {
                        startY = getYPosition(value, graphHeight, adjustY);
                    }

                    for (var k = 0; k < nodeList.length; k++) {
                        if (k != i && nodeList[k].data.length >= j && nodeList[k].data[j] == value) {
                            adjustY = i * graphLineWidth * 2 + 1;
                            break;
                        }
                    }

                    endX = startX - dataDrawingWidth;
                    endY = getYPosition(value, graphHeight, adjustY);

                    if ((value < 0 && node.data[j-1] > 0) ||
                            (value < 0 && j == 0) ||
                            (j!= nodeList.length -1 &&value < 0 && node.data[j+1] > 0)) {

                        context.moveTo(startX, startY);
                        context.fillStyle = node.color;
                        context.arc(startX, startY, dataDrawingWidth, 0, Math.PI * 2, true);
                        context.fill();
                    } else {
                        context.moveTo(startX, startY);
                        context.lineTo(endX, endY);
                    }

                    context.stroke();
                    startX = endX;
                    startY = endY;
                }
                context.closePath();

            }
        }

        function getYPosition(value, graphHeight, adjustY) {
            var pos = 0;
            if (value < 0)
                value = value * -1;

            pos = graphHeight - (graphHeight * (value / maxValue));

            if (pos < 0)
                pos = graphHeight;

            pos = pos + ((height - graphHeight) / 2);
            pos = pos + adjustY;

            return pos
        }

        onPaint:{
            drawBackground();
            drawGraphNode();
        }
    }

    Canvas {
        id: node
        width: parent.width
        height: parent.height
        anchors.fill: parent
    }

    Timer {
        id: refreshTimer
        interval: parent.refreshInterval
        repeat: true
        onTriggered: {
            Code.refreshGraphNode(graphName);
            background.requestPaint();
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

