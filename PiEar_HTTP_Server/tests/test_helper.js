const http = require('http');
const axios = require('axios');
async function make_request(endpoint, expected) {
    try {
        const resp = await axios.get(endpoint);
        let data = JSON.parse(resp.data);
        if (data != expected){
            console.error(`expected ${expected}, got ${data}`);
        }
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