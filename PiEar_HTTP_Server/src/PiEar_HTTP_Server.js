#!/usr/bin/env node
if (process.argv.length != 4) {
    console.warn(`Error: expected two arguments, found ${process.argv.length - 2}\nUsage: node index.js [Kill-Server-Secret] [Websocket-Shared-Secret]`);
    process.exit(1);
}

const { app } = require("./server");
