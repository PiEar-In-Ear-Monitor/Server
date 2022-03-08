#!/usr/bin/env node
//#region imports
const http = require('http');
const url = require('url');
var WebSocketServer = require('websocket').server;
//#endregion imports

function main() {

    //#region handle arguments and globals
    if (process.argv.length != 4) {
        console.warn(`Error: expected two arguments, found ${process.argv.length - 2}\nUsage: node index.js [Kill-Server-Secret] [Websocket-Shared-Secret]`);
        process.exit(1);
    }

    let is_initialized = false;
    const server = http.createServer(requestListener);
    server.listen(9090);
    const kill_secret = process.argv[2];
    const websocket_shared_secret = process.argv[3];
    let channel_names = { "Channels" : [] };
    let connection = null;

    console.log(`Server port: ${server.address().port}`);
    console.log(`Kill Endpoint: ${kill_secret}`);
    console.log(`Websocket Enpoint: ${websocket_shared_secret}`);
    //#endregion

    //#region endpoint handlers
    function handleBPMPut(urlParsed, res) {
        //#region Data validation
        if (urlParsed.query.bpm == null || !/[0-9]*/.test(urlParsed.query.bpm)) {
            res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
            res.end(JSON.stringify({"error": "expected query 'bpm' to be a number"}));
            console.warn(`Received bpm of ${urlParsed.query.bpm}`);
            return;
        }
        let new_bpm = parseInt(urlParsed.query.bpm);
        if (new_bpm < 0 || new_bpm > 255) {
            res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
            res.end(JSON.stringify({"error": "'bpm' expected to be betwen 0 and 255 (inclusive)"}));
            return;
        }
        //#endregion
        bpm = new_bpm;
        connection.sendUTF(JSON.stringify({"bpm": new_bpm}));
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
        channel_names.Channels.forEach(channel => {
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
        if (urlParsed.query.name != null && /[0-9]*/.test(urlParsed.query.name)) {
            new_name = urlParsed.query.name.slice(0, 26);
        } else {
            res.writeHead(422, { 'WebServer': 'PiEar-HTTP-Server' });
            res.end(JSON.stringify({"error": "expected a query, 'name', to consist of only numbers and letters."}));
            return;
        }
        let found_channel = false;
        channel_names.Channels.forEach(channel => {
            if (channel.piear_id == id) {
                channel.channel_name = new_name;
                connection.sendUTF(JSON.stringify({"piear_id": id, "channel_name": new_name}));
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

    //#region Websocket
    let wsServer = new WebSocketServer({
        httpServer: server,
        autoAcceptConnections: false,
        dropConnectionOnKeepaliveTimeout: false,
        parseCookies: false
    });

    function originIsAllowed(headers) {
        let userAgent = "";
        let SharedSecretSubmitted = "";
        headers.forEach((element, index, array) => {
            if (element == "User-Agent") {
                userAgent = array[index + 1];
            } else if (element == "Shared-Secret") {
                SharedSecretSubmitted = array[index + 1];
            }
        });
        return ((userAgent.search("PiEar-Server") != -1) && (SharedSecretSubmitted == websocket_shared_secret));
    }

    wsServer.on( 'request', function(request) {
        if (!originIsAllowed(request.httpRequest.rawHeaders)) {
            // Make sure we only accept requests from an allowed origin
            request.reject();
            return;
        }

        connection = request.accept(null, request.origin);
        
        connection.on('message', function(message) {
            if (!is_initialized) {
                let json = JSON.parse(message.utf8Data.replace('\u0000', ''));
                console.log(json);
                if (json.BPM != null) {
                    bpm = json.BPM;
                    is_initialized = true;
                } else {
                    let new_channel_name = json.channel_name;
                    let new_channel_id = json.piear_id;
                    channel_names.Channels.push({"piear_id": new_channel_id,"channel_name":new_channel_name});
                }
            } else {
                if (message.utf8Data == kill_secret) {
                    server.close();
                    wsServer.shutDown();
                } else {
                    console.log('Received Message: ' + message.utf8Data);
                }
            }
        });
        
        connection.on('close', function(reasonCode, description) {
            console.log("Websocket closed");
        });
    });
    //#endregion

    //#region server setup
    function requestListener (req, res) {
        const urlParsed = url.parse(req.url, true);
        if (urlParsed.pathname == "/abcdefghijklmnopqrstuvwxyz") {
            res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server'});
            res.end("zyxwvutsrqponmlkjihgfedcba");
            return;
        }
        if (!is_initialized) {
            res.writeHead(200);
            res.end(JSON.stringify({"error": "Server not initialized"}));
            return;
        }
        switch (urlParsed.pathname) {
        case "/bpm":
            (req.method == "PUT")?handleBPMPut(urlParsed, res):handleBPMGet(res);
            break;
        case "/channel-name":
            (req.method == "PUT")?handleChannelNamePut(urlParsed, res):handleChannelNameGet(urlParsed, res);
            break;
        default:
            res.writeHead(404, { 'WebServer': 'PiEar-HTTP-Server'});
            res.end(JSON.stringify({"error": `'${urlParsed.path}' not found`}));
            break;
        }
    };
    //#endregion
}

main();

module.exports = { main };
