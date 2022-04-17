function validBpm(bpm) {
    try {
        let final = parseInt(/^[0-9]{1,3}$/.exec(bpm)[0], 10);
        return (final === NaN)? null : final;
    } catch {
        return null;
    }
}

function validBpmEnabled(bpm_enabled) {
    try {
        let final = bpm_enabled.toLowerCase();
        if ((final === "true" || final === "false")) {
            return final;
        }
        return null;
    }
    catch {
        return null;
    }
}

function validNumber(number) {
    try {
        let final = parseInt(/^[0-9]*$/.exec(number)[0], 10);
        return ((typeof final === "number") && (final + "" !== "NaN"))? final : null;
    } catch {
        return null;
    }
}

function validChannelName(channel_name) {
    try {
        let final = (channel_name === null) ? null : /^[0-9a-zA-Z_]{1,26}/.exec(channel_name)[0];
        if (final === null || final.length === 0) {
            return null;
        } else {
            return (/[^0-9a-zA-Z_]/.test(channel_name)) ? null : final;
        }
    } catch {
        return null;
    }
}

function sendUpdates(app, data) {
    if (app.locals.ws_connection !== null) {
        app.locals.ws_connection.send(JSON.stringify(data));
    }
    app.locals.sse.forEach((sse) => {
        sse.send(JSON.stringify(data));
    });
}

module.exports = { 
    validBpm,
    validBpmEnabled,
    validNumber,
    validChannelName,
    sendUpdates
};
