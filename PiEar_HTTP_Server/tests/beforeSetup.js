#!/usr/bin/env node
const request = require('supertest');
const assert = require('assert');

async function beforeTest(app) {
    await request('http://localhost:9090')
        .get('/bpm')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.error, 'Server not initialized');
        })
    await request('http://localhost:9090')
        .get('/channel-name')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.error, 'Server not initialized');
        })
    await request('http://localhost:9090')
        .get('/does-not-exist')
        .expect('Content-Type', /json/)
        .expect(200)
        .then(response => {
            assert(response.body.error, 'Server not initialized');
        })
}

beforeTest("http://localhost:9090");
