const { make_request, make_change} = require('./test_helper');

function channel_name_tests(site) {
    for (let index = 0; index < 10; index++) {
        make_request(`${site}/channel-name?id=${index}`, JSON.stringify({"channel_name": `Channel ${index + 1}`}));
        make_change(`${site}/channel-name?id=${index}/Really-Cool-Name`);
    }
    for (let index = 0; index < 10; index++) {
        make_request(`${site}/channel-name?id=${index}`, JSON.stringify({"piear_id": index, "channel_name": `Really-Cool-Name`}));
    }
}

module.exports = { channel_name_tests };