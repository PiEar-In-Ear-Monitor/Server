#!/usr/bin/env node
if (process.argv.length != 3) {
    console.warn(`Error: expected argument, found ${process.argv.length - 2}\nUsage: node index.js [Websocket-Shared-Secret]`);
    process.exit(1);
}

const { app } = require("./server");
