#!/usr/bin/env node
if (process.argv.length != 4) {
    process.exit(1);
}

const { app } = require("./server");
