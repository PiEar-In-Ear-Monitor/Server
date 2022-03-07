#!/usr/bin/env node
var WebSocketClient = require('websocket').client;
const http = require('http')
console.log("Running tests");

http.get("http://localhost:9090/bpm", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

http.get("http://localhost:9090/channel-name", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

http.get("http://localhost:9090/does-not-exist", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

let client = new WebSocketClient();
let connection;
client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(con) {
    console.log('WebSocket Client Connected');
    connection = con;
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log("Received: '" + message.utf8Data + "'");
        }
    });
    for (let index = 0; index < 10; index++) {
        connection.sendUTF(JSON.stringify({"piear_id":index,"channel_name":`Channel ${index + 1}`}));
    }
    connection.sendUTF(JSON.stringify({"BPM":100}));
});

client.connect('ws://localhost:9090/', null, null, {"User-Agent":"PiEar-Server", "Shared-Secret":"hello"});

http.get("http://localhost:9090/bpm", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

http.get("http://localhost:9090/channel-name", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

http.get("http://localhost:9090/does-not-exist", (resp) => {
    let data = '';
    resp.on("data", (chunk) => {
        data += chunk;
    })

    resp.on('end', () => {
        if (JSON.parse(data)["error"] != "Server not initialized"){
            console.error('expected "Server not initialized", got ' + data);
        }
    })
}).on("error", (err) => {
    console.log("Error: " + err.message);
    process.exit(test_count);
});

