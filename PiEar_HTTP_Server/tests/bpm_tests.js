const { make_request, make_change} = require('./test_helper');

async function bpm_tests(site) {
    await Promise.resolve(make_request(`${site}/bpm`, {"bpmd": 100}, ["bpm"]));
    await Promise.resolve(make_change(`${site}/bpm/105`))
    await Promise.resolve(make_request(`${site}/bpm`, {"bpm": 105}, ["bpm"]));
    await Promise.resolve(make_change(`${site}/bpm/0`))
    await Promise.resolve(make_change(`${site}/bpm/-13`))
    await Promise.resolve(make_change(`${site}/bpm/326`))
    await Promise.resolve(make_change(`${site}/bpm/some-text`))
    await Promise.resolve(make_request(`${site}/bpm`, {"bpm": 105}, ["bpm"]));
}

module.exports = { bpm_tests };