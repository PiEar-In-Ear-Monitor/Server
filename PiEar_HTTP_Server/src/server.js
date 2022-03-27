#!/usr/bin/env node
const express = require('express');
const app = express();
require('express-ws')(app);
let cors = require('cors')
let { channelNameValidateId } = require("./Response_Helpers");
let websocket_shared_secret = "";
let ws_connection = null;
if (process.argv.length == 3) {
    websocket_shared_secret = process.argv[2];
}

app.use(express.json());
app.use(cors());
app.use(function(req, res, next) {
    res.header('WebServer', 'PiEar-HTTP-Sever');
    if (app.locals.bpm != -1 || req.url == "/abcdefghijklmnopqrstuvwxyz" || req.headers["upgrade"] != null) {
        next();
    } else {
        res.status(200).json({error: "Server not initialized"});            
        return;
    }
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
    if(ws_connection != null) {
        ws_connection.sendUTF(JSON.stringify({bpm: new_bpm}));
    }
    res.status(200).json({bpm: new_bpm});
});

app.put("/channel-name", (req, res) => {
    let id = channelNameValidateId(req.query.id);
    if ( id == false) {
        res.status(422).json({error: "expected a query, 'id', to be a number"});
        return;
    }
    let new_name;
    if (req.query.name != null && /[0-9]*/.test(req.query.name)) {
        new_name = req.query.name.slice(0, 26);
    } else {
        res.status(422).json({error: "expected a query, 'name', to consist of only numbers and letters."});
        return;
    }
    let found_channel = false;
    app.locals.channels.forEach(channel => {
        if (channel.piear_id == id) {
            channel.channel_name = new_name;
            if(ws_connection != null) {
                ws_connection.sendUTF(JSON.stringify({piear_id: id, channel_name: new_name}));
            }
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
    res.status(200).contentType('text/plain').send("zyxwvutsrqponmlkjihgfedcba");
});

app.listen(9090);

app.ws(`/${websocket_shared_secret}`, (ws, req) => {
    ws_connection = ws;
    ws.on('message', (msg) => {
        let message = msg.toString();
        if (app.locals.bpm == -1) {
            try {
                let json = JSON.parse(message.replace('\u0000', ''));
                if (json.bpm != null) {
                    app.locals.bpm = json.bpm;
                } else {
                    app.locals.channels.push({piear_id: json.piear_id,channel_name:json.channel_name});
                }
            } catch (e) {
                if (e == "fake") {
                    return;
                }
            }
        }
    });
    ws.on('close', () => {
        ws_connection = null;
    });
});

module.exports = { app };
