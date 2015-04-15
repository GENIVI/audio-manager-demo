.pragma library
var amSources = [];
var amSinks = [];
var amConnections = [];
var paSinkInputs = [];
var paSinks = [];
var paClients = [];
var colorListByName = [["MEDIA","red"], ["NAVI","green"],["skype","blue"],["TextToSpeach","orange"], ["reverse","cyan"]];

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

var graphDataset = [];
var colorList = ["red", "orange", "green", "blue", "black", "cyan"];
var colorListLength = colorList.length;

function Graph() {
    var name = "";
    var dataset;
    var nextColorIndex = 0;
}

function Node() {
    var nodeName = "";
    var color ="";
    var data;
    var id = -1;
    var removed = false;
}

function addGraphDataset(name) {
    var newGraphDataset = new Graph();

    newGraphDataset.name = name;
    newGraphDataset.dataset = new Array();
    newGraphDataset.nextColorIndex = 0;

    graphDataset[graphDataset.length] = newGraphDataset;
}

function removeGraphDataset(name) {
    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == name) {
            graphDataset.splice(i,1);
            break;
        }
    }
}

function addGraphNode(graphName, nodeName, id, maxDatalength, defaultValue) {
    var newNode = new Node();
    newNode.data = new Array(maxDatalength);

    for (var i = 0; i < maxDatalength; i++)
        newNode.data[i] = defaultValue;

    newNode.nodeName = nodeName;
    newNode.id = id;

    for (var i = 0; i < graphDataset.length; i++) {
        var graphdata = graphDataset[i];
        if (graphdata.name == graphName) {
            newNode.color = getGraphNodeColor(graphName, nodeName);
            graphDataset[i].dataset[graphdata.dataset.length] = newNode;
            return;
        }
    }

}

function getGraphNodeColor(graphName, nodeName) {
    console.log(colorList);
    for (var i = 0; i < colorListByName.length; i++) {
        if (nodeName == colorListByName[i][0]) {
            console.log("RETURN FIXED COLOR : " + nodeName + " / " + colorListByName[i][1]);

            return colorListByName[i][1];
        }
    }

    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == graphName) {
            var index = graphDataset[i].nextColorIndex;
            console.log("NEXT INDEX : " + index);

            if (index == colorListLength) {
                graphDataset[i].nextColorIndex = 0;
                return colorList[0];
            }
            graphDataset[i].nextColorIndex++;

            console.log("RETURN DEFAULT VALUE : " + colorList[index]);
            return colorList[index];
        }
    }

    console.log("RETURN DEFAULT VALUE : " + colorList[0]);
    return colorList[0];
}

function updateGraphNode(graphName, nodeName, id, maxDatalength, value) {

    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == graphName && graphDataset[i].dataset !== "undefined") {
            for (var j = 0; j < graphDataset[i].dataset.length; j++) {
                console.log("UPDATE GRAPH NODE : " + graphDataset[i].name + " //// " + graphName + " //// " + nodeName + graphDataset[i].dataset[j].nodeName);
                if (graphDataset[i].dataset[j].nodeName == nodeName) {
                    var nodeData = graphDataset[i].dataset[j];
                    var isRebirth = false;

                    // Node Rebirth
                    if (nodeData.id != id) {
                        console.log("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                        console.log("REBIRTH NODE : " + nodeName);
                        console.log("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                        var startIndexToFillMinus = graphDataset[i].dataset[j].data.length-1;
                        var filledData = graphDataset[i].dataset[j].data[startIndexToFillMinus -1] * -1;
                        for (var k = startIndexToFillMinus; k < maxDatalength-1; k++)
                            graphDataset[i].dataset[j].data[k] = filledData;
                        graphDataset[i].dataset[j].removed = false;
                        graphDataset[i].dataset[j].id = id;
                        isRebirth = true;
                    }

                    if (isRebirth || (!nodeData.removed && nodeData.id == id))
                        graphDataset[i].dataset[j].data[graphDataset[i].dataset[j].data.length] = value;

                    if (nodeData.data.length > maxDatalength)
                        graphDataset[i].dataset[j].data.splice(0,nodeData.data.length - maxDatalength);
                    return;
                }
            }
        }
    }

    addGraphNode(graphName, nodeName, id, maxDatalength, value);
}

function takeGraphDataset(graphName) {
    for (var i = 0; i < graphDataset.length; i++) {
        var dataset = graphDataset[i];
        if (dataset.name == graphName)
            return dataset.dataset;
    }
    return 0;
}

function getGraphNodeData(graphName, nodeName) {
    for (var i = 0; i < graphDataset.length; i++) {
        var dataset = graphDataset[i];

	if (dataset.name == graphName) {
	    for (var j = 0; j < dataset.dataset.length; j++) {
	        if (dataset.dataset[j].nodeName == nodeName)
		    return dataset.dataset[j];
	    }
	}
    }
}

function removeGraphNode(graphName, name, id) {

    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == graphName) {
            for (var j = 0; j < graphDataset[i].dataset.length; j++) {
                if (graphDataset[i].dataset[j].nodeName == name &&
                        graphDataset[i].dataset[j].id == id) {
                    console.log("REMOVE CHECK : " + graphName + " / " + name + " / " + id  );
                    graphDataset[i].dataset[j].removed = true;
                    return;
                }
            }
        }
    }
}

function eraseGraphNode(graphName, name, id) {

    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == graphName) {
            for (var j = 0; j < graphDataset[i].dataset.length; j++) {
                if (graphDataset[i].dataset[j].nodeName == name &&
                        graphDataset[i].dataset[j].id == id &&
                        graphDataset[i].dataset[j].removed) {
                    console.log("ERASE IT " + name + " / " + id );
                    graphDataset[i].dataset.splice(j,1);
                    return;
                }
            }
        }
    }
}
function refreshGraphNode(graphName) {
    for (var i = 0; i < graphDataset.length; i++) {
        if (graphDataset[i].name == graphName) {
            for (var j = 0; j < graphDataset[i].dataset.length; j++) {
                var lastIndex = graphDataset[i].dataset[j].data.length-1;
                var lastValue = graphDataset[i].dataset[j].data[lastIndex];
                if (lastIndex < 0) {
                    eraseGraphNode(graphName, graphDataset[i].dataset[j].nodeName, graphDataset[i].dataset[j].id);
                    return;
                }

                graphDataset[i].dataset[j].data.splice(0,1);

                if (!graphDataset[i].dataset[j].removed)
                    graphDataset[i].dataset[j].data[lastIndex] = lastValue;
            }
        }
    }
}


