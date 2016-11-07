var express = require('express');
var app = express();
var server = require('http').createServer(app);
var io = require('socket.io')(server);
var osc = require('node-osc'),io;

var oscServer, oscClient;
var firstTime = true;

server.listen(8080);
console.log('Local server established at port 8080');

// create fileserver
app.use(express.static(__dirname + '/HTML'));
app.get('/', function(req, res,next) {
    res.sendFile(__dirname + '/HTML/index.html');
});

// initialise socket and create OSC Server
io.sockets.on('connection', function (socket) {
	console.log('* page loaded')
    socket.on("config", function (obj) {
        if(firstTime == true){
            oscServer = new osc.Server(obj.server.port, obj.server.host);
            oscClient = new osc.Client(obj.client.host, obj.client.port);
            console.log('* new OSC Server  -  Port:'+obj.server.port+' IP:'+obj.server.host);
            console.log('* new OSC Client  -  Port:'+obj.client.port+' IP:'+obj.client.host);

            firstTime = false;
            //oscClient.send('/status', socket.sessionId + ' connected');
        }

        oscServer.on('message', function(msg, rinfo) {
            process.stdout.write('\r* received OSC Message: '+msg[0]+' '+msg[1]||' ');
            socket.emit("message", msg);
        });
    });
    socket.on("message", function (obj) {
        oscClient.send(obj[0], obj[1]||1);
        process.stdout.write('\r* sent OSC Message: '+obj[0]+' '+obj[1]||'x');

    });
});
