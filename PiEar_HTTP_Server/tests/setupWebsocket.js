#!/usr/bin/env node
const WebSocket = require('ws');
let ws = new WebSocket(`ws://localhost:9090/ws_test`);

ws.on('open', function open() {
    ws.send(JSON.stringify({"piear_id": 1, "channel_name":"Channel 1"}), () => {
        ws.send(JSON.stringify({"piear_id": 2, "channel_name":"Channel 2"}), () => {
            ws.send(JSON.stringify({"piear_id": 3, "channel_name":"Channel 3"}), () => {
                ws.send(JSON.stringify({"piear_id": 4, "channel_name":"Channel 4"}), () => {
                    ws.send(JSON.stringify({"bpm": 100}), () => {
                        ws.close();
                    });
                });
            });
        });
    });
});
