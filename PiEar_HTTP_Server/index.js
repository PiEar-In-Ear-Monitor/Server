#!/usr/bin/env node
//#region imports
const http = require('http');
const url = require('url');
const server = http.createServer(requestListener);
server.listen(9090);
//#endregion imports

//#region handle arguments and globals
if (process.argv.length != 4) {
    console.warn(`Error: expected two arguments, found ${process.argv.length - 2}\nUsage: node index.js [Kill-Server-Endpoint] [Path-To-Channel-Info]`);
    process.exit(1);
}

console.log(`Server addres: ${server.address().address}\nServer port: ${server.address().port}`)
const kill_endpoint = process.argv[2];
let channel_names = parse_channel_file(process.argv[3]);

function parse_channel_file(path_to_json_file) {
    return {
        "channels": [
            {
                "name": "channel 1",
                "enabled": true
            },
            {
                "name": "channel 2",
                "enabled": true
            },
            {
                "name": "channel 3",
                "enabled": true
            }
        ]
    }
}

//#endregion

//#region endpoint handlers
function handleBPM(urlParsed, res) {   
    res.end(JSON.stringify({"path": urlParsed.query.id}));
}

function handleChannelName(urlParsed, res) {
    res.end(JSON.stringify({"path": urlParsed.query.id}));
}
//#endregion

//#region server setup
function requestListener (req, res) {
    const urlParsed = url.parse(req.url, true);
    res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
    switch (urlParsed.pathname) {
    case "/bpm":
        handleBPM(urlParsed, res);
        break;
    case "/channel-name":
        handleChannelName(urlParsed, res);
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