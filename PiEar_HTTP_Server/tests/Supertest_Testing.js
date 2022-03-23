#!/usr/bin/env node
const request = require('supertest');
const { app } = require('../src/server');
const assert = require('assert');
const  WebSocket = require('ws').WebSocket;

const ws = new WebSocket('ws://localhost:9090');

function runTests() {
    request(app)
        .get('/bpm')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.bpm, '100');
        })
        .catch(err => console.log(err));
    request(app)
        .put('/bpm?bpm=200')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.bpm, '200');
        })
        .catch(err => console.log(err.message));
    request(app)
        .get('/channel?id=2')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Channel 2')
        })
        .catch(err => console.log(err));
    request(app)
        .put('/channel?id=2&name=Hello')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Hello')
        })
        .catch(err => console.log(err));
    // request(app)
    //     .put('/DOESN-T_EXIST')
    //     .expect('Content-Type', /json/)
    //     .expect(200)
    //     .then(response => {
    //         assert(response.body.email, 'foo@bar.com')
    //         done();
    //     })
    //     .catch(err => console.log(err));
}

ws.on('open', function open() {
    console.log('Initializing WebServer');
    ws.send(JSON.stringify({"piear_id": 1, "channel_name":"Channel 1"}), () => {
        ws.send(JSON.stringify({"piear_id": 2, "channel_name":"Channel 2"}), () => {
            ws.send(JSON.stringify({"piear_id": 3, "channel_name":"Channel 3"}), () => {
                ws.send(JSON.stringify({"piear_id": 4, "channel_name":"Channel 4"}), () => {
                    ws.send(JSON.stringify({"bpm": 100}), () => {
                        runTests();
                        // ws.send("kill");
                    });
                });
            });
        });
    });
    
});

ws.on('message', function message(data) {
    console.log('received: %s', data);
});
