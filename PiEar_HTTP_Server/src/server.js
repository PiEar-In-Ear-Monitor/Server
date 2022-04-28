#!/usr/bin/env node
const express = require("express");
const app = express();
require("express-ws")(app);
const cors = require("cors");
const { validBpm, validBpmEnabled, validChannelName, validNumber, sendUpdates } = require("./Response_Helpers");

app.use(express.json());
app.use(cors());
app.use(function(req, res, next) {
    res.header("WebServer", "PiEar-HTTP-Sever");
    if (app.locals.bpm !== -1 || req.url === "/abcdefghijklmnopqrstuvwxyz" || req.headers["shared-secret"] === app.locals.wsSecret) {
        next();
    } else {
        res.status(200).json({error: "Server not initialized"});            
        return;
    }
});

app.locals.bpm = -1;
app.locals.bpmEnabled = "false";
app.locals.channels = [];
app.locals.sse = [];
app.locals.wsConnection = null;
app.locals.wsSecret = (process.argv.length === 3)? process.argv[2] : "";

app.get("/bpm", (req, res) => {
    res.status(200).json({bpm: app.locals.bpm, bpm_enabled: app.locals.bpmEnabled});
});

app.get("/channel-name", (req, res) => {
    if (Object.keys(req.query).length === 0) {
        res.status(200).json({channel_count: app.locals.channels.length});
        return;
    }
    let id = validNumber(req.query.id);
    if ( id === null) {
        res.status(422).json({error: "expected a query, \"id\", to be a number"});
        return;
    }
    let final = app.locals.channels.filter((channel) => channel.piear_id === id);
    if (final.length === 0) {
        res.status(422).json({error: `Cannot find channel with id ${id}`});
        return;
    }
    res.status(200).json({channel_name: final[0].channel_name});
});

function formPutBpmResponse(bpm, enabled) {
    let final = {};
    if (enabled !== null) {
        app.locals.bpmEnabled = enabled;
        final.bpm_enabled = enabled;
    }
    if (bpm !== null) {
        app.locals.bpm = bpm;
        final.bpm = bpm;
    }
    return final;
}

function putBpm(req, res) {
    let final = formPutBpmResponse(validBpm(req.query.bpm), validBpmEnabled(req.query.bpmEnabled));
    if (Object.keys(final).length === 0) {
        res.status(422).json({error: "expected a query, \"bpmEnabled\", or \"bpm\", to be a number"});
        return;
    }
    sendUpdates(app.locals.wsConnection, app.locals.sse, final);
    res.status(200).json(final);
}

app.put("/bpm", (req, res) => { putBpm(req, res); });

function putChannelName(req, res) {
    let id = validNumber(req.query.id);
    if ( id === null) {
        res.status(422).json({error: "expected a query, \"id\", to be a number"});
        return;
    }
    let newName = validChannelName(req.query.name);
    if (newName === null) {
        res.status(422).json({error: "expected a query, \"name\", to consist of only numbers and letters."});
        return;
    }
    let final = app.locals.channels.filter((chan) => chan.piear_id === id);
    if (final.length !== 1) {
        res.status(422).json({error: `Cannot find channel with id ${id}`});
        return;
    }
    final[0].channel_name = newName;
    sendUpdates(app.locals.wsConnection, app.locals.sse, {piear_id: id, channel_name: newName});
    res.status(200).json({channel_name: newName});
}


app.put("/channel-name", (req, res) => { putChannelName(req, res); });

app.get("/abcdefghijklmnopqrstuvwxyz", (req, res) => {
    res.status(200).contentType("text/plain").send("zyxwvutsrqponmlkjihgfedcba");
});

app.listen(9090);

function handleWs(ws) {
    app.locals.wsConnection = ws;
    ws.on("message", (msg) => {
        let message = msg.toString();
        if (app.locals.bpm === -1) {
            try {
                let json = JSON.parse(message.replace("\u0000", ""));
                if (typeof json.bpm !== "undefined") {
                    app.locals.bpm = json.bpm;
                } else {
                    app.locals.channels.push({piear_id: json.piear_id, channel_name:json.channel_name});
                }
            } catch (e) {
                return;
            }
        }
    });
    ws.on("close", () => {
        app.locals.wsConnection = null;
    });
}

app.ws("/", (ws, req) => {
    handleWs(ws);
});

app.get("/channel-name/listen", (req, res) => {
    res.set({
        "Cache-Control": "no-cache",
        "Content-Type": "text/event-stream",
        "Connection": "keep-alive"
    });
    res.flushHeaders();
    app.locals.sse.push(res);
    res.on("close", () => {app.locals.sse = app.locals.sse.filter((sse) => sse !== res);});
});

module.exports = { app };
