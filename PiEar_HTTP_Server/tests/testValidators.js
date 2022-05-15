#!/usr/bin/env node
const { validBpm, validBpmEnabled, validChannelName, validNumber } = require("../src/Response_Helpers");
const assert = require("assert");

function testBpm() {
    assert(validBpm(null) === null);
    assert(validBpm("") === null);
    assert(validBpm("Some") === null);
    assert(validBpm("String") === null);
    assert(validBpm("2 Numbers Involved 1") === null);
    assert(validBpm("284 ") === null);
    for (let i = 0; i < 1000; i++) {
        assert(validBpm(String(i)) === i);        
    }
    for (let i = 1000; i < 10000; i++) {
        assert(validBpm(String(i)) === null);
    }
    for (let i = -1; i > -10000; i--) {
        assert(validBpm(String(i)) === null);
    }
}

function testBpmEnabled() {
    assert(validBpmEnabled(null) === null);
    assert(validBpmEnabled("") === null);
    assert(validBpmEnabled("Some") === null);
    assert(validBpmEnabled("String") === null);
    assert(validBpmEnabled("true") === "true");
    assert(validBpmEnabled("false") === "false");
    assert(validBpmEnabled("True") === "true");
    assert(validBpmEnabled("False") === "false");
    assert(validBpmEnabled("TRUE") === "true");
    assert(validBpmEnabled("FALSE") === "false");
    assert(validBpmEnabled("True ") === null);
    assert(validBpmEnabled("False ") === null);
}

function testNumber() {
    assert(validNumber(null) === null);
    assert(validNumber("") === null);
    assert(validNumber("Some") === null);
    assert(validNumber("String") === null);
    assert(validNumber("2 Numbers Involved 1") === null);
    assert(validNumber("284 ") === null);
    for (let i = 0; i < 10000; i++) {
        assert(validNumber(String(i)) === i);        
    }
    for (let i = -1; i > -10000; i--) {
        assert(validBpm(String(i)) === null);
    }
}

function testChannelName() {
    assert(validChannelName(null) === null);
    assert(validChannelName("") === null);
    assert(validChannelName(Buffer.from("Some").toString('base64')) === Buffer.from("Some").toString('base64'));
    assert(validChannelName(Buffer.from("Some Spaces").toString('base64')) === Buffer.from("Some Spaces").toString('base64'));
    assert(validChannelName(Buffer.from("String").toString('base64')) === Buffer.from("String").toString('base64'));
    assert(validChannelName(Buffer.from("2_Numbers_Involved_1").toString('base64')) === Buffer.from("2_Numbers_Involved_1").toString('base64'));
    assert(validChannelName(Buffer.from("284 ").toString('base64')) === Buffer.from("284 ").toString('base64'));
    assert(validChannelName(Buffer.from("0123456789________________").toString('base64')) === Buffer.from("0123456789________________").toString('base64'));
    assert(validChannelName(Buffer.from("abcdefghijklmnopqrstuvwxyz").toString('base64')) === Buffer.from("abcdefghijklmnopqrstuvwxyz").toString('base64'));
    assert(validChannelName(Buffer.from("abcdefghijklmnopqrstuvwxyz0123456789_").toString('base64')) === Buffer.from("abcdefghijklmnopqrstuvwxyz0123456789_").toString('base64'));
}

function testValidators() {
    testBpm();
    testBpmEnabled();
    testNumber();
    testChannelName();
}

testValidators();
