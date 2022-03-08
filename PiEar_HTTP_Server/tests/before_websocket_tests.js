const {make_request} = require('./test_helper');

function before_ws_tests(site) {
    make_request(`${ site }/bpm`, JSON.stringify({"error": "Server not initialized"}));
    make_request(`${ site }/channel-name`, JSON.stringify({"error": "Server not initialized"}));
    make_request(`${ site }/does-not-exist`, JSON.stringify({"error": "Server not initialized"}));
}

module.exports = { before_ws_tests };