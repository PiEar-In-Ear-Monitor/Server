#!/usr/bin/env node
let express = require('express');
let cors = require('cors')
const ws = require('ws');
let { channelNameValidateId } = require("./Response_Helpers");

const kill_secret = process.argv[2];
const websocket_shared_secret = process.argv[3];
let wsServer = new ws.Server({ noServer: true });
let ws_connection = null;

console.log(`kill: ${kill_secret}`);
console.log(`shared_secret: ${websocket_shared_secret}`);

const app = express();
app.use(express.json());
app.use(cors());
app.use(function(req, res, next) {
    res.header('WebServer', 'PiEar-HTTP-Sever');
    if (app.locals.bpm != -1) {
        next();
        return;
    }
    res.status(200).json({error: "Server not initialized"});
});
app.locals.bpm = -1;
app.locals.channels = [];
app.get("/bpm", (req, res) => {
    res.status(200).json({bpm: app.locals.bpm});
});

app.get("/channel-name", (req, res) => {
    let id = channelNameValidateId(req.query.id);
    if ( id == false) {
        res.status(422).json({error: "expected a query, 'id', to be a number"});
        return;
    }
    let found_channel = false;
    app.locals.channels.forEach(channel => {
        if (channel.piear_id == id) {
            res.status(200).json({channel_name: channel.channel_name});
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.status(422).json({error: `Cannot find channel with id ${id}`});
    }
});

app.put("/bpm", (req, res) => {
    //#region Data validation
    if (req.query.bpm == null || !/[0-9]*/.test(req.query.bpm)) {
        res.status(422).json({error: "expected query 'bpm' to be a number"});
        console.warn(`Received bpm of ${req.query.bpm}`);
        return;
    }
    let new_bpm = parseInt(req.query.bpm);
    if (new_bpm < 0 || new_bpm > 255) {
        res.status(422).json({error: "'bpm' expected to be betwen 0 and 255 (inclusive)"});
        return;
    }
    //#endregion
    app.locals.bpm = new_bpm;
    // connection.sendUTF(JSON.stringify({"bpm": new_bpm})); // TODO SEND OVER WS
    res.status(200).json({bpm: new_bpm});
});

app.put("/channel-name", (req, res) => {
    let id = channelNameValidateId(req.query.id);
    if ( id == false) {
        res.status(422).json({"error": "expected a query, 'id', to be a number"});
        return;
    }
    let new_name;
    if (req.query.name != null && /[0-9]*/.test(req.query.name)) {
        new_name = req.query.name.slice(0, 26);
    } else {
        res.status(422).json({"error": "expected a query, 'name', to consist of only numbers and letters."});
        return;
    }
    let found_channel = false;
    app.locals.channels.forEach(channel => {
        if (channel.piear_id == id) {
            channel.channel_name = new_name;
            // connection.sendUTF(JSON.stringify({"piear_id": id, "channel_name": new_name})); // TODO SEND OVER WS
            res.status(200).json({channel_name: channel.channel_name});
            found_channel = true;
            return;
        }
    });
    if (!found_channel) {
        res.status(422).json({error: `Cannot find channel with id ${id}`});
    }
});

app.get("/abcdefghijklmnopqrstuvwxyz", (req, res) => {
    res.contentType("text/plain").send("zyxwvutsrqponmlkjihgfedcba"); 
});

const server = app.listen(9090);

function wsHeaderChecker(headers) {
    let host = headers["host"];
    let user_agent = headers["user-agent"];
    let shared_secret = headers["shared-secret"];
    console.log(`host: ${host}, user-agent: ${user_agent}, shared_secret: ${shared_secret}`);
    return true;
    return (
        (host == "localhost:9090") &&
        (user_agent.indexOf("PiEar-Server") != -1) && 
        (shared_secret == websocket_shared_secret)
    );
}

server.on('upgrade', (request, socket, head) => {
    if (wsHeaderChecker(request.headers)) {
        wsServer.handleUpgrade(request, socket, head, socket => {
            wsServer.emit('connection', socket, request);
        });
    }
});

wsServer.on('connection', socket => {
    ws_connection = socket;
    socket.on('message', msg => {
        let message = msg.toString();
        if (message == kill_secret) {
            socket.close();
            process.exit(0);
        }
        if (app.locals.bpm == -1) {
            try {
                let json = JSON.parse(message.replace('\u0000', ''));
                if (json.bpm != null) {
                    app.locals.bpm = json.bpm;
                } else {
                    app.locals.channels.push({"piear_id": json.piear_id,"channel_name":json.channel_name});
                }
            } catch (e) {
                console.log(e);
                console.log('Try/Catch Message: ' + message);
            }
        }
    });
});

module.exports = { app };
