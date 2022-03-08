#!/usr/bin/env node
var WebSocketClient = require('websocket').client;
const http = require('http')
const {before_ws_tests} = require("./before_websocket_tests");
const {bpm_tests} = require("./bpm_tests");
const {channel_name_tests} = require("./channel_name_tests");
var childProcess = require('child_process');

let server_process = childProcess.spawn("../server.js kill-test-secret header-test-secret");

console.log("Running tests");

before_ws_tests("http://localhost:9090");

let client = new WebSocketClient();
let connection;
client.on('connectFailed', function(error) {
    // console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(con) {
    // console.log('WebSocket Client Connected');
    connection = con;
    connection.on('error', function(error) {
        // console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        // console.log('Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            // console.log("Received: '" + message.utf8Data + "'");
        }
    });
    for (let index = 0; index < 10; index++) {
        connection.sendUTF(JSON.stringify({"piear_id":index,"channel_name":`Channel ${index + 1}`}));
    }
    connection.sendUTF(JSON.stringify({"BPM":100}));

    bpm_tests("http://localhost:9090");
    channel_name_tests("http://localhost:9090");
    connection.sendUTF("kill-test-secret");
    connection.close();
});

client.connect('ws://localhost:9090/', null, null, {"User-Agent":"PiEar-Server", "Shared-Secret":"header-test-secret"});

console.log("Done testing");