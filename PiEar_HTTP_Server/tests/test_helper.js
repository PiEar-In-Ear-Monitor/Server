const http = require('http');
const axios = require('axios');
const exp = require('constants');
async function make_request(endpoint, expected, keys) {
    try {
        const resp = await axios.get(endpoint);
        let data = JSON.parse(resp.data);
        console.log(data);
        keys.forEach(key => {
            if (data[key] != expected[key]) {
                console.error(`expected ${key} to be ${expected[key]}, got ${data[key]}`);
            }
        });
    } catch (error) {
    }
}

async function make_change(endpoint) {
    try {
        const resp = await axios.put(endpoint, null);
    } catch (error) {

    }
}

module.exports = { make_request, make_change };