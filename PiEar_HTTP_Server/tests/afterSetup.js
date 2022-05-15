#!/usr/bin/env node
const request = require("supertest");
const assert = require("assert");

async function afterSetup() {
    await request("http://localhost:9090")
        .get("/bpm")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.bpm, "100");
            assert(response.body.bpm_enabled, "false");
        });
    await request("http://localhost:9090")
        .put("/bpm")
        .expect("Content-Type", /json/)
        .expect(422)
        .then((response) => {
            assert(response.body.error, "expected a query, \"bpmEnabled\", or \"bpm\", to be a number");
        });
    await request("http://localhost:9090")
        .put("/bpm?bpm=200")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.bpm, "200");
        });
    await request("http://localhost:9090")
        .get("/channel-name?id=2")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "Q2hhbm5lbCAy");
        });
    await request("http://localhost:9090")
        .put("/channel-name?id=1&name=U29tZV9OYW1l") // Some_Name
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "U29tZV9OYW1l");
        });
    await request("http://localhost:9090")
        .put("/channel-name?id=2&name=S2V5Ym9hcmQ=") // Keyboard
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "S2V5Ym9hcmQ=");
        });
    await request("http://localhost:9090")
        .put("/channel-name?id=3&name=R3VpdGFy") // Guitar
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "R3VpdGFy");
        });
    await request("http://localhost:9090")
        .put("/channel-name?id=4&name=TmV3X0NoYW5uZWw=") // New_Channel
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "TmV3X0NoYW5uZWw=");
        });
    await request("http://localhost:9090")
        .put("/endpoint_dne")
        .expect("Content-Type", /text/)
        .expect(404)
        .then((response) => {
            assert(response.text, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n<title>Error</title>\n</head>\n<body>\n<pre>Cannot PUT /endpoint_dne</pre>\n</body>\n</html>");
        });
    await request("http://localhost:9090")
        .put("/bpm?bpmEnabled=True")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.bpm_enabled, "true");
        });
    await request("http://localhost:9090")
        .get("/bpm")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.bpm, "200");
            assert(response.body.bpm_enabled, "true");
        });
    await request("http://localhost:9090")
        .get("/channel-name?id=1")
        .expect("Content-Type", /json/)
        .expect(200)
        .then((response) => {
            assert(response.body.channel_name, "Some_Name");
        });
}

afterSetup("http://localhost:9090");
