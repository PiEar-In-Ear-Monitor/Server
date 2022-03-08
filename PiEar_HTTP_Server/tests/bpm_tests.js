const { make_request, make_change} = require('./test_helper');

function bpm_tests(site) {
    make_request(`${site}/bpm`, {"bpm": 100});
    make_change(`${site}/bpm/105`)
    make_request(`${site}/bpm`, {"bpm": 105});
    make_change(`${site}/bpm/0`)
    make_change(`${site}/bpm/-13`)
    make_change(`${site}/bpm/326`)
    make_change(`${site}/bpm/some-text`)
    make_request(`${site}/bpm`, {"bpm": 105});
}

module.exports = { bpm_tests };