#!/usr/bin/env node
import { createServer } from 'http';

function handleBPM(req, res) {
    
}

function handleChannelName(req, res) {
    
}


const requestListener = function (req, res) {
    res.writeHead(200, { 'WebServer': 'PiEar-HTTP-Server' });
    if (req.url.search("/bpm") != -1) {
        handleBPM(req, res);
    } else if (req.url.search("/channel-name") != -1) {
        handleChannelName(req, res);
    }
    res.writeHead(404, { 'WebServer': 'PiEar-HTTP-Server'});
    res.end();
  }
  
const server = createServer(requestListener);
server.listen(9090);