#!/usr/bin/env node
let express = require('express');
let cors = require('cors')
const ws = require('ws');
var WebSocketServer = require('websocket').server;
let { channelNameValidateId } = require("./Response_Helpers");

if (process.argv.length != 4) {
    console.warn(`Error: expected two arguments, found ${process.argv.length - 2}\nUsage: node index.js [Kill-Server-Secret] [Websocket-Shared-Secret]`);
    process.exit(1);
}

let is_initialized = false;
const kill_secret = process.argv[2];
const websocket_shared_secret = process.argv[3];
let channel_names = [];
let ws_connection = null;
let bpm = 100;
let wsServer = new ws.Server({ noServer: true });
const app = express();
app.use(express.json());
app.use(cors());
app.use(function(req, res, next) {
    res.header('WebServer', 'PiEar-HTTP-Sever');
    res.contentType((req.url.indexOf("abcdefghijklmnopqrstuvwxyz") == -1)?"application/json":"text/plain");
    next();
});

app.get("/bpm", (req, res) => {
    res.status(200);
    res.send({"bpm": bpm});
});

app.get("/channel-name", (req, res) => {
    let id = channelNameValidateId(req.query.id);
    if ( id == false) {
        res.status(422);
        res.send({"error": "expected a query, 'id', to be a number"});
        return;
    }
    let found_channel = false;
    channel_names.Channels.forEach(channel => {
        if (channel.piear_id == id) {
            res.status(200);
            res.send({'channel_name': channel.channel_name});
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.status(422);
        res.send({"error": `Cannot find channel with id ${id}`});
    }
});

app.put("/bpm", (req, res) => {
    //#region Data validation
    if (res.query.bpm == null || !/[0-9]*/.test(res.query.bpm)) {
        res.status(422);
        res.send({"error": "expected query 'bpm' to be a number"});
        console.warn(`Received bpm of ${res.query.bpm}`);
        return;
    }
    let new_bpm = parseInt(res.query.bpm);
    if (new_bpm < 0 || new_bpm > 255) {
        res.status(422);
        res.send({"error": "'bpm' expected to be betwen 0 and 255 (inclusive)"});
        return;
    }
    //#endregion
    bpm = new_bpm;
    connection.sendUTF(JSON.stringify({"bpm": new_bpm}));
    res.status(200);
    res.send({"bpm": bpm});
});

app.put("/channel-name", (req, res) => {
    let id = channelNameValidateId(req.query.id);
    if ( id == false) {
        res.status(422);
        res.send({"error": "expected a query, 'id', to be a number"});
        return;
    }
    let new_name;
    if (req.query.name != null && /[0-9]*/.test(req.query.name)) {
        new_name = req.query.name.slice(0, 26);
    } else {
        res.status(422);
        res.send({"error": "expected a query, 'name', to consist of only numbers and letters."});
        return;
    }
    let found_channel = false;
    channel_names.Channels.forEach(channel => {
        if (channel.piear_id == id) {
            channel.channel_name = new_name;
            connection.sendUTF(JSON.stringify({"piear_id": id, "channel_name": new_name}));
            res.status(200);
            res.send({'channel_name': channel.channel_name});
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.status(422);
        res.send({"error": `Cannot find channel with id ${id}`});
    }
});

app.get("/abcdefghijklmnopqrstuvwxyz", (req, res) => {
    res.send("zyxwvutsrqponmlkjihgfedcba");
});

const server = app.listen(9090);

server.on('upgrade', (request, socket, head) => {
    wsServer.handleUpgrade(request, socket, head, socket => {
        wsServer.emit('message', request);
    });
});

function originIsAllowed(headers) {
    let userAgent = headers["user-agent"];
    let SharedSecretSubmitted = headers["shared-secret"];
    return ((userAgent.indexOf("PiEar-Server") != -1) && (SharedSecretSubmitted == websocket_shared_secret));
}


wsServer.on('message', function(message) {
    console.log(message);
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
            process.exit(0);
        } else {
            console.log('Received Message: ' + message.utf8Data);
        }
    }
});

wsServer.on('close', function(reasonCode, description) {
    console.log("Websocket closed");
});

module.exports = { app };
