#!/usr/bin/env node

const request = require('supertest');
const assert = require('assert');
const express = require('express');
const { app } = require('../src/PiEar_HTTP_Server');

request(app)
    .get('/bpm')
    .expect('Content-Type', /json/)
    .expect(200)
    .end(function(err, res) {
        if (err) throw err;
});
