import QtQuick 2.1
import QtQuick.Window 2.0
import com.windriver.ammonitor 1.0
import "code.js" as Code

Item {
    id: mainBox

    PAClient {
        id: paClient
        signal sinkInputProcessed(int processType, int index)
        signal sinkInfoProcessed(int processType, int index)

        onSinkInputChanged : {

            if (sinkinput.role && sinkinput.role != "event" && sinkinput.role != "filter") {
                console.log('onSinkInputChanged '+sinkinput.index+' Volume'+sinkinput.volume);
                Code.savePASinkInput(sinkinput);
                architectureDiagram.requestPaint();
                pulseaudioChart.updateData(sinkinput.role, sinkinput.index, sinkinput.volume);

                console.log('----');
                for (var prop in sinkinput)
                    console.log("Object item:", prop, "=", sinkinput[prop])
                console.log('----');
                sinkInputProcessed(0, sinkinput.index);
            }
        }
        onSinkInputRemoved : {
            console.log('onSinkInputRemoved '+index);
            var sinkinput = Code.takePASinkInput(index);

            if (sinkinput && sinkinput.role != "event") {
                console.log('----');
                for (var prop in sinkinput)
                    console.log("Object item:", prop, "=", sinkinput[prop])
                console.log('----');

                pulseaudioChart.removeData(sinkinput.role, sinkinput.index);
            }
            sinkInputProcessed(1, index);
	    architectureDiagram.requestPaint();

        }

        onSinkInfoChanged: {
            console.log("onSinkInfoChanged " + sinkinfo.index + " Volume " + sinkinfo.volume);
            Code.savePASinkInfo(sinkinfo);
	    architectureDiagram.requestPaint();

            audiomanagerChart.updateData(sinkinfo.name, sinkinfo.index, sinkinfo.volume);
            sinkInfoProcessed(0, sinkinfo.index);
        }

        onSinkInfoRemoved: {
            console.log("onSinkInfoRemoved " + index);
            var sinkinfo = Code.takePASinkInfo(index);
            audiomanagerChart.removeData(sinkinfo.name, sinkinfo.index);
	    architectureDiagram.requestPaint();

        }

        onClientChanged: {
            console.log("onClientChanged " + client.index + " name " + client.name);
            Code.savePAClient(client);
	    architectureDiagram.requestPaint();
        }

        onClientRemoved: {
            console.log("onClientRemoved " + index);
            var client = Code.takePAClient(index);
	    architectureDiagram.requestPaint();

        }
    }

    AMClient {
        id: amClient
        property bool initialized: false
        onSinkAdded: {
	    // skip default AM Sinks
	    if(sink.name.substr(0, 2) == "my")
		return;
	    if(!initialized) {
		Code.amSinks[Code.amSinks.length] = sink
		console.log("SINK ADDED : " + sink.id + " / " + sink.name);
	    }
        }

        onSinkRemoved: {

        }

        onVolumeChanged: {
            console.log("**********************************");
            console.log("QML : VOLUME CHANGED : SINKID = " + sinkid + " / VOLUME = " + volume);
            console.log("**********************************");
        }

        onSourceAdded: {
	    // skip default AM Sources
	    if(source.name.substr(0, 2) == "my")
		return;
	    if(!initialized) {
		Code.amSources[Code.amSources.length] = source
		console.log("SOURCE ADDED : " + source.id + " / " + source.name);
	    }
        }

        onSourceRemoved: {

        }

        onConnectionAdded: {
            console.log("**********************************");
            console.log("QML : CONNECTION : " + connection.id+ " "+initialized);
            console.log("**********************************");
	    if(!initialized) {
		// remove previous connection
		amClient.disconnect(connection.id);
		return;
	    }
            Code.saveAMConnection(connection);
        }

        onConnectionRemoved: {
            console.log("**********************************");
            console.log("QML : CONNECTION REMOVED : " + index);
            console.log("**********************************");
	    var conn = Code.takeAMConnection(index);
        }

	onInitAMMonitorCompleted: {
	    console.log("onInitDBusCallCompleted");
	    amClient.initialized = true;
	}

    }


    Rectangle {
        id: buttonPanel
        color: "transparent"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: parent.height / 40
        width: parent.width / 6
        height: parent.height
        property int buttonWidth : width * 9 / 10
        property int buttonHeight : height / 7

        ListView {
            id: buttonsView

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            orientation: ListView.Vertical

            model: VisualItemModel {
		Text {
		    text: "Audio Players of IVI"
		    font.pixelSize: parent.height / 30
		    anchors.horizontalCenter: parent.horizontalCenter
		}
                Button {
                    width: buttonPanel.buttonWidth
                    height: buttonPanel.buttonHeight

                    iconName: "music"
                    title: "Media"

                    amCommandIF: amClient
                    amSource: "MediaPlayer"
                    amSink: "AlsaPrimary"
                    mediaRole: "MEDIA"
                    audioFilePath: "audio/media.wav"
                }

                Button {
                    width: buttonPanel.buttonWidth
                    height: buttonPanel.buttonHeight

                    iconName: "car"
                    title: "Navi"

                    amCommandIF: amClient
                    amSource: "NaviPlayer"
                    amSink: "AlsaSecondary"
                    mediaRole: "NAVI"
                    audioFilePath: "audio/navigation.wav"
                }

                Button {
                    width: buttonPanel.buttonWidth
                    height: buttonPanel.buttonHeight

                    iconName: "phone"
                    title: "Phone"

                    amCommandIF: amClient
                    amSource: "Skype"
                    amSink: "AlsaSecondary"
                    mediaRole: "skype"
                    audioFilePath: "audio/telephone-ring.wav"
                }

                Button {
                    width: buttonPanel.buttonWidth
                    height: buttonPanel.buttonHeight

                    iconName: "microphone"
                    title: "TTS"

                    amCommandIF: amClient
                    amSource: "TTSPlayer"
                    amSink: "AlsaSecondary"
                    mediaRole: "TextToSpeach"
                    audioFilePath: "audio/tts.wav"
                }

                Button {
                    width: buttonPanel.buttonWidth
                    height: buttonPanel.buttonHeight

                    iconName: "reply"
                    title: "Reverse"

                    amCommandIF: amClient
                    amSource: "ReverseBeep"
                    amSink: "AlsaSecondary"
                    mediaRole: "reverse"
                    audioFilePath: "audio/car_reverse.wav"
                }
		Text {
		    text: "(Press audio play button to\nconnect the AM source and sink)"
		    font.pixelSize: buttonPanel.height / 60
		    anchors.horizontalCenter: parent.horizontalCenter
		    height: 50
		}
            }
        }
    }

    Rectangle {
        id: amVolumeChartPanel
        anchors.left: buttonPanel.right
        anchors.top: parent.top
        width: parent.width * 2 / 6
        height: parent.height / 2

        /*
        Text {
            text: "50"
            font.pixelSize: parent.height / 20
            anchors.right: audiomanagerChart.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 5
        }
        */
        Graph {
            id: audiomanagerChart
            graphName : "AudiomanagerChart"
            title: "Sinks of GENIVI® Audio Manager"
	    description: "AM's Sink volume changes by Control Plugin"
            anchors.fill: parent
            defaultValue : 0
            maxDataLength: 100
            width: parent.width
            height: parent.height
        }
    }

    Rectangle {
        id: pulseaudioVolumeChartPanel
        anchors.left: buttonPanel.right
        anchors.top: amVolumeChartPanel.bottom
        width: parent.width * 2 / 6
        height: parent.height / 2

        Graph {
            id: pulseaudioChart
            title: "Sink Inputs of PulseAudio"
	    description: "PA's Sink Input volume changes"
            anchors.fill: parent
            graphName : "PulseAudioChart"
            defaultValue : 0
            maxDataLength: 100
            width: parent.width
            height: parent.height
        }
    }

    Rectangle {
        id: dialogPanel
        anchors.left: amVolumeChartPanel.right
        anchors.top: parent.top
        width: parent.width * 3 / 6
        height: parent.height


	Canvas {
	    id: architectureDiagram
	    anchors.fill: parent
	    property string geniviLogo:"images/genivi-logo.png"
	    property string pulseaudioLogo:"images/pulseaudio-logo.png"
	    property string speaker:"images/speaker.png"
	    Component.onCompleted: {
		loadImage(geniviLogo);
		loadImage(pulseaudioLogo);
		loadImage(speaker);
	    }
	    function plugInSocket(x, y, name, length, rotate) {
		var ctx = getContext('2d');
		var r = 5;
		var w = 16;
		var h = 16;
		ctx.save();

		// adjust position
		if(rotate == 90) {
		    ctx.translate(x+w, y-h/2)
		    ctx.rotate(Math.PI/2)
		} else if(rotate == 180) {
		    ctx.translate(x-w/2, y-h)
		} else { // 0
		    ctx.translate(x+w/2, y+h)
		    ctx.rotate(Math.PI) // default degree
		}
		length -= h

		ctx.strokeStyle = "#222222"
		ctx.lineWidth = 1.0
		ctx.fillStyle = "#0099ff"
		ctx.font = "12px sans-serif"

		ctx.beginPath();
		// top
		ctx.lineTo(w-r,0);
		// draw top right corner
		ctx.arcTo(w,0,w,r,r);
		ctx.lineTo(w,h);    // right side
		ctx.lineTo(0,h);              // bottom side
		ctx.lineTo(0,r);                 // left side
		// draw top left corner
		ctx.arcTo(0,0,r,0,r);
		ctx.closePath();
		ctx.fill();
		ctx.stroke();


		// plug
		ctx.fillStyle = "white"
		var plugW = 4
		var plugH = 7
		ctx.beginPath();
		ctx.lineTo(w/3 - plugW/2, h);
		ctx.lineTo(w/3 - plugW/2, h+plugH);
		ctx.lineTo(w/3 + plugW/2, h+plugH);
		ctx.lineTo(w/3 + plugW/2, h);
		ctx.closePath();
		ctx.fill();
		ctx.stroke();

		ctx.beginPath();
		ctx.lineTo(w*2/3 - plugW/2, h);
		ctx.lineTo(w*2/3 - plugW/2, h+plugH);
		ctx.lineTo(w*2/3 + plugW/2, h+plugH);
		ctx.lineTo(w*2/3 + plugW/2, h);
		ctx.closePath();
		ctx.fill();
		ctx.stroke();

		ctx.beginPath();
		ctx.lineTo(w/2 - plugW/2, 0);
		ctx.lineTo(w/2 - plugW/2, - length);
		ctx.lineTo(w/2 + plugW/2, - length);
		ctx.lineTo(w/2 + plugW/2, 0);
		ctx.closePath();
		ctx.fill();
		ctx.stroke();


		ctx.restore();

		ctx.save()
		ctx.translate(x, y)
		ctx.rotate(0)
		ctx.fillStyle = "black";
		var m = ctx.measureText(name);
		var tx = (w - m.width)/2, ty = length
		if(rotate == 90)
		    tx = (w - m.width)/2, ty = length/3
		else if(rotate == 180)
		    tx = (w - m.width)/2, ty = - length/3
		ctx.fillText(name, tx, ty)
		ctx.restore();

	    }
	    function roundRect(x, y, w, h, name, verticalCenter,initCB,postCB) {
		var ctx = getContext('2d');
		var r = 10;
		ctx.save();
		ctx.translate(x, y)

		var fontSize = 12;
		ctx.font = ""+fontSize+"px sans-serif"

		if(initCB != null)
		    initCB(ctx);
		else {
		    ctx.strokeStyle = "#222222"
		    ctx.lineWidth = 1.0
		    ctx.fillStyle = "lightGray"
		}

		ctx.beginPath();
		// top
		ctx.lineTo(w-r,0);
		// draw top right corner
		ctx.arcTo(w,0,w,r,r);
		ctx.lineTo(w,h-r);    // right side
		// draw bottom right corner
		ctx.arcTo(w,h,w-r,h,r);
		ctx.lineTo(r,h);              // bottom side
		// draw bottom left corner
		ctx.arcTo(0,h,0,h-r,r);
		ctx.lineTo(0,r);                 // left side
		// draw top left corner
		ctx.arcTo(0,0,r,0,r);

		ctx.closePath();
		ctx.fill();
		ctx.stroke();

		ctx.fillStyle = "black";
		var m = ctx.measureText(name);
		if(verticalCenter)
		    ctx.fillText(name, (w - m.width)/2, (h+fontSize)/2);
		else
		    ctx.fillText(name, (w - m.width)/2, 16);

		if(postCB != null)
		    postCB(ctx);

		ctx.restore();

	    }
	    function audioManager() {
		this.w=parent.width*2/3, this.h=parent.height*2/5
		this.x = (parent.width - this.w)/2 - this.w/10, this.y = parent.height/6

		var logoWidth = this.h/2
		var logoHeight = this.h/2
		var logoX = (this.w - logoWidth)/2
		var logoY = (this.h - logoHeight)/2
		roundRect(this.x, this.y, this.w, this.h, "GENIVI® Audio Manager",
		    false, null,
		    function(ctx) {
			ctx.drawImage(geniviLogo, logoX, logoY, logoWidth, logoHeight);
		    });


		// AM Sources
		var ctx = getContext('2d');
		var bx = this.x + this.w/16;
		var by = this.y+this.h/10;
		var tw = this.w/4
		var th = this.h/8
		var sourceJCX = bx+tw
		var sourceJCY = by+10
		ctx.fillText(Code.amSources.length+" AM Source(s)", bx, by)
		for(var i=0; i<Code.amSources.length; i++) {
		    var source = Code.amSources[i];
		    var tx = bx;
		    var ty = by+10;
		    roundRect(tx, ty+(th+th/4)*i, tw, th, source.name, true,
			function(ctx) {
			    ctx.lineWidth = 1.0
			    ctx.fillStyle = "#99ccff"
			}
		    );
		}

		// AM Sinks
		bx = this.x+this.w-tw-this.w/16
		by = this.y+this.h/10;
		var sinkJCX = bx
		var sinkJCY = by+10
		ctx.fillText(Code.amSinks.length+" AM Sink(s)", bx, by)
		for(var i=0; i<Code.amSinks.length; i++) {
		    var sink = Code.amSinks[i];
		    var tx = bx;
		    var ty = by+10
		    roundRect(tx, ty+(th+th/4)*i, tw, th, sink.name, true,
			function(ctx) {
			    ctx.lineWidth = 1.0
			    ctx.fillStyle = "#99ccff"
			}
		    );
		}

		// Connection
		for(var i=0; i<Code.amConnections.length; i++) {
		    var conn = Code.amConnections[i];
		    var source = null;
		    var sink = null;
		    for(var j=0; j<Code.amSources.length; j++) {
			var src = Code.amSources[j];
			if(src.id == conn.sourceId) {
			    source = src;
			    source.idx = j
			    break;
			}
		    }
		    for(var j=0; j<Code.amSinks.length; j++) {
			var snk = Code.amSinks[j];
			if(snk.id == conn.sinkId) {
			    sink = snk;
			    sink.idx = j
			    break;
			}
		    }
		    if(!source || !sink)
			continue;
		    ctx.fillText(" Connection id: "+conn.id, sourceJCX,sourceJCY+(th+th/4)*source.idx+th/2-2);
		    ctx.beginPath();
		    ctx.moveTo(sourceJCX,sourceJCY+(th+th/4)*source.idx+th/2);
		    ctx.lineTo(sinkJCX,sinkJCY+(th+th/4)*sink.idx+th/2);
		    ctx.closePath();
		    ctx.fill();
		    ctx.stroke();
		}

	    }
	    function pulseaudio() {
		this.w=parent.width/3, this.h=(parent.height*7/24)
		this.x = (parent.width - this.w)*2/3, this.y = parent.height*15/24
		var logoWidth = this.w*3/4
		var logoHeight = 85*logoWidth/470; // logo 480px x 85px
		var logoX = (this.w - logoWidth)/2
		var logoY = this.h / 30

		roundRect(this.x, this.y, this.w, this.h, "", false,
		    null,
		    function(ctx) {
			ctx.drawImage(pulseaudioLogo, logoX, logoY, logoWidth, logoHeight);
		    });

		var ctx = getContext('2d');
		// sink devices
		var tw = this.w*3/4
		var th = this.h/9
		var tx = this.x + (this.w - tw)/2;
		var ty = this.y + this.h*2/7;
		var sinkJCX = tx;
		var sinkJCY = ty;
		var sinkH = th;

		var bw = (this.w*2/3);
		var bx = this.x - bw*3/2;
		var by = this.y-this.h/4;
		ctx.fillText(Code.paSinks.length+" PA Sink(s)", tx, ty-5)
		for(var i=0; i<Code.paSinks.length; i++) {
		    var sink = Code.paSinks[i];
		    roundRect(tx, ty+(th+th/4)*i, tw, th, sink.name, true,
			function(ctx) {
			    ctx.lineWidth = 1.0
			    ctx.fillStyle = "#99ccff"
			}
		    );
		    ctx.save();
		    ctx.strokeStyle = Code.getGraphNodeData("AudiomanagerChart", sink.name).color;
		    ctx.beginPath();
		    ctx.moveTo(tx+tw, ty+(th+th/4)*i+th/2);
		    ctx.lineTo(tx+tw+tw*2/5, ty+(th+th/4)*i+th/2);
		    ctx.closePath();
		    ctx.stroke();

		    ctx.drawImage(speaker, tx+tw+tw*2/5, ty+(th+th/4)*i, th, th)
		    ctx.restore();

		}


		// find active PA clients
		var activeClients = [];
		for(var i=0; i<Code.paClients.length; i++) {
		    var client = Code.paClients[i];
		    for(var j=0; j<Code.paSinkInputs.length; j++) {
			var sinkInput = Code.paSinkInputs[j];
			if(sinkInput.clientIndex == client.index) {
			    client.sinkInput = sinkInput
			    activeClients[activeClients.length] = client;
			    break;
			}
		    }
		}

		// draw active PA clients
		var bw = (this.w*3/4);
		var bx = this.x - bw*3/2;
		var by = this.y-this.h/10;
		ctx.fillText(activeClients.length+" PA's Active Client(s)", bx, by)
		for(var i=0; i<activeClients.length; i++) {
		    var client = activeClients[i];
		    var tw = bw;
		    var th = this.h/5
		    var tx = bx;
		    var ty = this.y - this.h/12;
		    roundRect(tx, ty+(th+th/4)*i, tw, th,
			client.name, false,
			function(ctx) {
			    ctx.lineWidth = 1.0
			    ctx.fillStyle = "#99ccff"
			},
			function(ctx) {
			    var mediaRole = 'media.role='+client.sinkInput.role;
			    var m = ctx.measureText(mediaRole);
			    ctx.fillText(mediaRole, (tw - m.width)/2, th-6);
			}
		    );
		    ctx.save();
		    // sink input line to sink
		    for(var j=0; j<Code.paSinks.length; j++) {
			var sink = Code.paSinks[j];
			if(sink.index == client.sinkInput.sinkIndex) {
			    ctx.fillText(" Stream idx: "+sinkInput.index, tx+tw,ty+(th+th/4)*i+th/2);
			    ctx.strokeStyle = Code.getGraphNodeColor("PulseAudioChart", client.sinkInput.role);
			    ctx.beginPath();
			    ctx.moveTo(tx+tw,ty+(th+th/4)*i+th/2);
			    ctx.lineTo(sinkJCX, sinkJCY+(sinkH+sinkH/4)*j+sinkH/2);
			    ctx.closePath();
			    ctx.stroke();
			    break;
			}
		    }
		    ctx.restore();
		}
	    }
	    onPaint: {
		var ctx = getContext('2d');
		ctx.save();
		ctx.clearRect(0, 0, width, height);
		var am = new audioManager();
		var pa = new pulseaudio();
		plugInSocket(am.x+am.w*2/3, am.y+am.h,
			    "Routing Plugin for PulseAudio", pa.y-(am.y+am.h))

		plugInSocket(am.x+am.w/2, am.y,
			    "Command Plugin for D-Bus", am.y*3/4, 180)

		roundRect(am.x+am.w/2-parent.width/3/2, am.y/4,
			    parent.width/3, parent.height/30,
			    "Audio Manager Monitor", true);

		plugInSocket(am.x+am.w, am.y+am.h/2,
			    "Control Plugin", am.y*3/4, 90)

		roundRect(am.x+am.w+am.w/14, am.y+am.h/2-(parent.height/30)/2,
			    parent.width/6, parent.height/30,
			    "Audio Policy", true);

		ctx.restore();
	    }
	}
	Text {
	    text: "(LIVE Architecture Diagram)"
	    font.pixelSize: parent.height / 40
	    anchors.bottom: dialogPanel.bottom
	    anchors.bottomMargin: parent.height/60
	    anchors.horizontalCenter: parent.horizontalCenter
	}
    }
}
