const http = require('http');
const axios = require('axios');
function make_request(endpoint, expected) {
    http.get(endpoint, (resp) => {
        let data = '';
        resp.on("data", (chunk) => {
            data += chunk;
        })

        resp.on('end', () => {
            if (data != expected){
                console.error(`expected ${expected}, got ${data}`);
            }
        })
    }).on("error", (err) => {
        console.log("Error: " + err.message);
    });
}

function make_change(endpoint) {
    axios.put('endpoint', null).then((res) => {
    }).catch((err) => {
        // console.error(err);
    });
}

module.exports = { make_request, make_change };