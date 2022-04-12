#!/usr/bin/env node
const request = require('supertest');
const assert = require('assert');

async function afterSetup(app) {
    await request('http://localhost:9090')
        .get('/bpm')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.bpm, '100');
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/bpm?bpm=200')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.bpm, '200');
        })
        .catch(err => console.log(err.message));
    await request('http://localhost:9090')
        .get('/channel-name?id=2')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Channel 2')
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/channel-name?id=1&name=Some_Name')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Some_Name')
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/channel-name?id=2&name=Keyboard')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Keyboard')
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/channel-name?id=3&name=Guitar')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'Guitar')
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/channel-name?id=4&name=New_Channel')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.channel_name, 'New_Channel')
        })
        .catch(err => console.log(err));
    await request('http://localhost:9090')
        .put('/endpoint_dne')
        .expect('Content-Type', /text/)
        .expect(404)
        .then(response => {
            assert(response.text, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n<title>Error</title>\n</head>\n<body>\n<pre>Cannot PUT /endpoint_dne</pre>\n</body>\n</html>");
        })
        .catch(err => console.log(err));
}

afterSetup("http://localhost:9090");