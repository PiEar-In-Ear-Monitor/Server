const { make_request, make_change} = require('./test_helper');

async function channel_name_tests(site) {
    for (let index = 0; index < 10; index++) {
        await make_request(`${site}/channel-name?id=${index}`, {"channel_name": `Channel ${index + 1}`}, ["channel_name"]);
        await make_change(`${site}/channel-name?id=${index}&name=Really-Cool-Name`);
    }
    for (let index = 0; index < 10; index++) {
        await make_request(`${site}/channel-name?id=${index}`, {"channel_name": "Really-Cool-Name"}, ["channel_name"]);
    }
    // make_change(`${site}/channel-name?id=${index}/Really-Cool-Name`);
    // make_request(`${site}/channel-name?id=${index}`, {"channel_name": `Channel ${index + 1}`});
}

module.exports = { channel_name_tests };