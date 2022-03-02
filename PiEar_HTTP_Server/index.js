#!/usr/bin/env node
//#region imports
const http = require('http');
const url = require('url');
var WebSocketServer = require('websocket').server;
//#endregion imports

//#region handle arguments and globals
if (process.argv.length != 6) {
    console.warn(`Error: expected four arguments, found ${process.argv.length - 2}\nUsage: node index.js [Kill-Server-Endpoint] [Websocket-Endpoint] [Path-To-Channel-Info] [BPM]`);
    process.exit(1);
}

const server = http.createServer(requestListener);
server.listen(9090);
const kill_endpoint = process.argv[2];
const websocket_endpoint = process.argv[3];
let channel_count = 0;
let channel_names = parse_channel_file(process.argv[4]);
let bpm = parseInt(process.argv[5]);
console.log(`Server port: ${server.address().port}`);
console.log(`Kill Endpoint: ${kill_endpoint}`);
console.log(`Websocket Enpoint: ${websocket_endpoint}`);
console.log(`Number of Channels: ${channel_names.channels.length}`);
console.log(`Current BPM: ${bpm}`);

function parse_channel_file(path_to_json_file) {
    return {
        "channels":
            [
                {
                    "pipewire_id":0,
                    "piear_id":0,
                    "channel_name":"Channel 0",
                    "enabled":false
                },
                {
                    "pipewire_id": 1,
                    "piear_id":2,
                    "channel_name":"Channel 1",
                    "enabled":true
                },
                {
                    "pipewire_id":2,
                    "piear_id":4,
                    "channel_name":"Channel 2",
                    "enabled":true
                }
            ]
    }
}
//#endregion

//#region endpoint handlers
function handleBPMPut(urlParsed, res) {
    //#region Data validation
    if (urlParsed.query.new_bpm == null || !/[0-9]*/.test(urlParsed.query.new_bpm)) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": "expected query 'new_bpm' to be a number"}));
        console.warn(`Received new_bpm of ${urlParsed.query.new_bpm}`);
        return;
    }
    let new_bpm = parseInt(urlParsed.query.new_bpm);
    if (new_bpm < 0 || new_bpm > 255) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": "'new_bpm' expected to be betwen 0 and 255 (inclusive)"}));
        return;
    }
    //#endregion
    bpm = new_bpm;
    res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
    res.end(JSON.stringify({"bpm": bpm}));
}

function handleBPMGet(res) {
    res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
    res.end(JSON.stringify({"bpm": bpm}));
}

function channelNameValidateId(id) {
    if (id != null && /[0-9]*/.test(id)) {
        return parseInt(id);
    }
    return false;
}

function handleChannelNameGet(urlParsed, res) {
    let id = channelNameValidateId(urlParsed.query.id);
    if ( id == false) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": "expected a query, 'id', to be a number"}));
        return;
    }
    let found_channel = false;
    channel_names.channels.forEach(channel => {
        if (channel.piear_id == id) {
            res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
            res.end(JSON.stringify({'channel_name': channel.channel_name}));
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": `Cannot find channel with id ${id}`}));    
    }
}

function handleChannelNamePut(urlParsed, res) {
    let id = channelNameValidateId(urlParsed.query.id);
    if ( id == false) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": "expected a query, 'id', to be a number"}));
        return;
    }
    let new_name;
    if (urlParsed.query.new_name != null && /[0-9]*/.test(urlParsed.query.new_name)) {
        new_name = urlParsed.query.new_name.slice(0, 26);
    } else {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": "expected a query, 'new_name', to consist of only numbers and letters."}));
        return;
    }
    let found_channel = false;
    channel_names.channels.forEach(channel => {
        if (channel.piear_id == id) {
            channel.channel_name = new_name;
            res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
            res.end(JSON.stringify({'channel_name': channel.channel_name}));
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
        res.end(JSON.stringify({"error": `Cannot find channel with id ${id}`}));    
    }
}
//#endregion

//#region server setup
function requestListener (req, res) {
    const urlParsed = url.parse(req.url, true);
    switch (urlParsed.pathname) {
    case "/bpm":
        (req.method == "PUT")?handleBPMPut(urlParsed, res):handleBPMGet(res);
        break;
    case "/channel-name":
        (req.method == "PUT")?handleChannelNamePut(urlParsed, res):handleChannelNameGet(urlParsed, res);
        break;
    case "/kill-server":
        res.end(JSON.stringify({"kill-command": true}));
        server.close();
        break;
    default:
        res.writeHead(404, { 'WebServer': 'PiEar-HTTP-Server'});
        res.end(JSON.stringify({"error": `'${urlParsed.path}' not found`}));
        break;
    }
};
//#endregion

//#region Websocket
let wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false,
    dropConnectionOnKeepaliveTimeout: false,
});

function originIsAllowed(origin) {
    console.log(origin);
    return true;
}

wsServer.on( 'request', function(request) {
    if (!originIsAllowed(request.origin)) {
        // Make sure we only accept requests from an allowed origin
        request.reject();
        console.log((new Date()) + ' Connection from origin ' + request.origin + ' rejected.');
        return;
    }

    var connection = request.accept('echo-protocol', request.origin);
    console.log((new Date()) + ' Connection accepted.');
    
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log('Received Message: ' + message.utf8Data);
            connection.sendUTF(message.utf8Data);
        }
        else if (message.type === 'binary') {
            console.log('Received Binary Message of ' + message.binaryData.length + ' bytes');
            connection.sendBytes(message.binaryData);
        }
    });
    
    connection.on('close', function(reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
    });
});
//#endregion
