const {make_request} = require('./test_helper');

async function before_ws_tests(site) {
    await make_request(`${ site }/bpm`, JSON.stringify({"error": "Server not initialized"}), ["error"]);
    await make_request(`${ site }/channel-name`, JSON.stringify({"error": "Server not initialized"}), ["error"]);
    await make_request(`${ site }/does-not-exist`, JSON.stringify({"error": "Server not initialized"}), ["error"]);
}

module.exports = { before_ws_tests };