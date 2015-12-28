.pragma library
var amSources = [];
var amSinks = [];
var amConnections = [];
var paSinkInputs = [];
var paSinks = [];
var paClients = [];

function saveAMConnection(connection) {
    var exist = false;


    for (var i = 0; i < amConnections.length; i++) {
        var info = amConnections[i];

        if (info.id == connection.id) {
            exist = true;
            amConnections[i] = connection;
            break;
        }
    }

    if (!exist)
        amConnections[amConnections.length] = connection;
    return connection;
}

function takeAMConnection(id) {
    for (var i = 0; i < amConnections.length; i++) {
        var info = amConnections[i];

        if (info.id == id) {
            amConnections.splice(i,1);
            return info;
        }
    }

    return null;
}

function findAMConnection(id) {
    for (var i = 0; i < amConnections.length; i++) {
        var info = amConnections[i];

        if (info.id == id) {
            return amConnections[i];
        }
    }
    return null;
}

function savePAClient(client) {
    var exist = false;

    for (var i = 0; i < paClients.length; i++) {
        var info = paClients[i];

        if (info.index == client.index) {
            exist = true;
            paClients[i] = client;
            break;
        }
    }

    if (!exist)
        paClients[paClients.length] = client;
    return client;
}

function takePAClient(index) {
    for (var i = 0; i < paClients.length; i++) {
        var info = paClients[i];

        if (info.index == index) {
            paClients.splice(i,1);
            return info;
        }
    }

    return null;
}

function findPAClient(index) {
    for (var i = 0; i < paClients.length; i++) {
        var info = paClients[i];

        if (info.index == index) {
            return paClients[i];
        }
    }

    return null;
}


function savePASinkInfo(sinkinfo) {
    var exist = false;

    for (var i = 0; i < paSinks.length; i++) {
        var info = paSinks[i];

        if (info.index == sinkinfo.index) {
            exist = true;
            paSinks[i] = sinkinfo;
            break;
        }
    }

    if (!exist)
        paSinks[paSinks.length] = sinkinfo;
    return sinkinfo;
}

function takePASinkInfo(index) {
    for (var i = 0; i < paSinks.length; i++) {
        var info = paSinks[i];

        if (info.index == index) {
            paSinks.splice(i,1);
            return info;
        }
    }

    return null;
}

function findPASinkInfo(index) {
    for (var i = 0; i < paSinks.length; i++) {
        var info = paSinks[i];

        if (info.index == index) {
            return paSinks[i];
        }
    }

    return null;
}

function findPASinkInput(index) {
    for(var i=0; i<paSinkInputs.length; i++) {
        var si = paSinkInputs[i];
        if(si.index == index)
            return si;
    }
    return null;
}

function savePASinkInput(sinkinput) {
    var exist = false;
    for(var i=0; i<paSinkInputs.length; i++) {
        var si = paSinkInputs[i];
        if(si.index == sinkinput.index) {
            exist = true;
            paSinkInputs[i] = sinkinput;
            break;
        }
    }
    if(!exist)
        paSinkInputs[paSinkInputs.length] = sinkinput;
    return sinkinput;
}


function takePASinkInput(index) {
    for(var i=0; i<paSinkInputs.length; i++) {
        var si = paSinkInputs[i];
        if(si.index == index) {
            paSinkInputs.splice(i, 1);
            return si;
        }
    }
    return null;
}

var GraphType = {
    CONTINUOUS_LINE: 1,
    TRANSIENT_LINE: 2,
};

var GraphLineColor = {
    colorList : ["red", "orange", "green", "blue", "black", "cyan"],
    colorListByRole : [["MEDIA","red"],
                       ["NAVI","green"],
                       ["skype","blue"],
                       ["TextToSpeach","orange"],
                       ["reverse","cyan"]]
}

var Graph = function(type) {
    var graph = this;

    this.type = type;

    this.colorList = GraphLineColor.colorList;
    this.colorListByRole = GraphLineColor.colorListByRole;

    this.nodeList = new Array();
    this.nextColorIndex = 0;

    this.width = 0;
    this.height = 0;
    this.backgroundColor = 0;
    this.maxValue = 0;
    this.maxDataLength = 0;
    this.lineWidth = 0;
    this.backgroundLineColor = 0;
    this.backgroundLineWidth = 0;

    this.loadGraphInfo = function(info) {
        this.width = info.width;
        this.height = info.height;
        this.maxValue = info.maxValue;
        this.lineWidth = info.lineWidth;
        this.maxDataLength = info.maxDataLength;
        this.backgroundColor = info.backgroundColor;
        this.backgroundLineColor = info.backgroundLineColor;
        this.backgroundLineWidth = info.backgroundLineWidth;
    }

    this.drawBackground = function(context) {
        var width = this.width;
        var height = this.height;
        var bgColor = this.backgroundColor;
        var bgLineColor = this.backgroundLineColor;
        var bgLineWidth = this.backgroundLineWidth;

        // Fill background color
        context.save();

        // Draw Guide Line
        context.translate(0.8,0.8);
        var horizontalWidth = width / 10;
        var verticalWidth = height / 10;

        for(var i = 0; i < 10; i++) {
            if (i == 0)
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
        context.moveTo(0, (verticalWidth * 8) + 7);
        context.lineTo(width, (verticalWidth * 8) + 7);
        context.closePath();
        context.stroke();
        context.restore();
    }

    this.drawGraphNode = function(context) {
        var graphWidth = this.width - 2;
        var graphHeight = this.height * (0.6);
        var graphLineWidth = this.lineWidth;
        var nodeList = this.nodeList;
        var lineColor = "black";
        var dataDrawingWidth = graphWidth / this.maxDataLength;

        if (nodeList.length == 0)
            return;

        context.lineWidth = graphLineWidth;

        for (var i = 0; i < nodeList.length; i++) {
            var node = nodeList[i];
            var startX = graph.width-1;
            var startY = graph.height-1;
            var endX = 0;
            var endY = 0;

            context.beginPath();
            lineColor = node.color;
            context.strokeStyle = lineColor;
            if (node.removed)
                startX = dataDrawingWidth * node.data.length;

            var offsetY = getOffsetY(node);
            for (var j = node.data.length-1; j > 0; j--) {
                var value = node.data[j];

                if (j == node.data.length-1)
                    startY = getYPosition(value, graphHeight, offsetY);

                endX = startX - dataDrawingWidth;
                endY = getYPosition(value, graphHeight, offsetY);

                if (value < 0) {
                    startX = endX;
                    startY = endY;
                    continue;
                }

                context.moveTo(startX, startY);
                context.lineTo(endX, endY);
                context.stroke();

                startX = endX;
                startY = endY;
            }

            context.closePath();
        }
    }

    this.checkGraphNodeInList = function(nodeName, id) {
        for (var index = 0; index < graph.nodeList.length; index++) {
            var node = graph.nodeList[index];
            if (node.nodeName == nodeName && node.id == id) {
                console.log("Found GraphNode, nodeName = " + nodeName +
                    " id = " + id);
                return index;
            }
        }

        return -1;
    }

    this.updateGraphNode = function(index, value) {
        var node = this.nodeList[index];
        var lastLength = node.data.length;
        node.data[lastLength-1] = value;

        console.log("Try to update Volume(" + value + ")" + " for " + node.nodeName);
    }

    this.addGraphNode = function(nodeName, id, maxDatalength, value) {
        var newNode = new Node(nodeName, id, value, maxDatalength);

        newNode.data = new Array(
                        (this.type == GraphType.TRANSIENT_LINE) ?
                        1 : maxDatalength);

        for (var i = 0; i < newNode.data.length; i++)
            newNode.data[i] = value;

        newNode.color = getGraphListColor(nodeName);
        this.nodeList.push(newNode);
    }

    this.removeGraphNode = function(index) {
        this.nodeList[index].removed = true;

        if (this.type == GraphType.TRANSIENT_LINE)
            insertPaddingDataInGraphNode(this.nodeList[index]);
    }

    this.getGraphNodeColor = function(nodeName) {
        var nodeColor = null;

        for (var i = 0; i < this.nodeList.length; i++) {
            if (this.nodeList[i].nodeName == nodeName)
                    nodeColor = this.nodeList[i].color;
        }

        return nodeColor;
    }

    this.refreshGraphNode = function() {
        for (var i = 0; i < this.nodeList.length; i++) {
            var node = this.nodeList[i];
            var lastIndex = node.data.length - 1;
            var lastValue = node.data[lastIndex];

            if (node.data.length == 0) {
                eraseGraphNode(node.nodeName, node.id);
                return;
            }

            switch (this.type) {
                case GraphType.CONTINUOUS_LINE:
                {
                    node.data.shift();
                    node.data.push(lastValue);
                    break;
                }

                case GraphType.TRANSIENT_LINE:
                {
                    if (node.removed) {
                        node.data.shift();
                        break;
                    }

                    if (node.data.length > this.maxDataLength) {
                        console.log("Info: OverFlow data buffer. Try to delete front element");
                        node.data.shift();
                    }

                    node.data.push(lastValue);

                    break;
                }

                default:
                    console.log("Error: Invalid GraphType in refreshGraphNode");
            }
        }
    }

    function insertPaddingDataInGraphNode(node) {
        var currentLength = node.data.length;
        var remainingLength = graph.maxDataLength - currentLength;
        for (var i = 0 ; i < remainingLength ; i++)
            node.data.unshift(-1);
    }

    function eraseGraphNode(nodeName, id) {
        var index = graph.checkGraphNodeInList(nodeName, id);

        if (index < 0) return;

        if (graph.nodeList[index].removed) {
            console.log("Erase it " + nodeName + " / " + id );
            graph.nodeList.splice(index, 1);
        }
    }

    function getGraphListColor(nodeName) {
        var index;

        for (index = 0; index < graph.colorListByRole.length; index++) {
            if (nodeName == graph.colorListByRole[index][0]) {
                console.log("RETURN FIXED COLOR : " +
                    nodeName + " / " + graph.colorListByRole[index][1]);
                return graph.colorListByRole[index][1];
            }
        }

        index = graph.nextColorIndex;

        if (index == graph.colorList.length) {
            console.log("OverFlow color list for node");
            graph.nextColorIndex = 0;
            return graph.colorList[0];
        }

        graph.nextColorIndex++;

        console.log("RETURN DEFAULT VALUE : " + graph.colorList[index]);
        return graph.colorList[index];
    }

    function getOffsetY(node) {
        for (var i=0 ; i < graph.colorList.length; i++) {
            if (node.color == graph.colorList[i])
                 return i * graph.lineWidth * 2 + 1;
        }

        console.log("Error: Cannot get offset for  " + node);

        return -1;
    }

    function getYPosition(value, graphHeight, offsetY) {
        var pos = 0;

        if (value < 0)
            value = value * -1;

        pos = graphHeight - (graphHeight * (value / graph.maxValue));

        if (pos < 0)
            pos = graphHeight;

        pos = pos + ((graph.height - graphHeight) / 2);
        pos = pos + offsetY;

        return pos
    }
}

var Node = function(nodeName, id, value, maxDatalength) {
    this.nodeName = nodeName;
    this.id = id;
    this.removed = false;
    this.nextColorIndex = 0;
    this.data = null;
}
