#!/usr/bin/env node
if (process.argv.length !== 3) {
    process.exit(1);
}

require("./server");
