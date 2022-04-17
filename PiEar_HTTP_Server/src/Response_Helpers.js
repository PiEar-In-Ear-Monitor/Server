function validBpm(bpm) {
    try {
        let final = parseInt(/^[0-9]{1,3}$/.exec(bpm)[0], 10);
        return (isNaN(final))? null : final;
    } catch {
        return null;
    }
}

function validBpmEnabled(bpmEnabled) {
    if (typeof bpmEnabled === "string") {
        let final = bpmEnabled.toLowerCase();
        if ((final === "true" || final === "false")) {
            return final;
        }
    }
    return null;
}

function validNumber(number) {
    try {
        let final = parseInt(number, 10);
        return ((typeof final === "number") && !isNaN(final) && (String(final) === number)) ? final : null;
    } catch {
        return null;
    }
}

function validChannelName(channelName) {
    let final = (channelName === null) ? -1 : String(/^[0-9a-zA-Z_]{1,26}/.exec(channelName));
    return (channelName === final) ? final : null ;
}

function sendUpdates(ws, sseArray, data) {
    if (ws !== null) {
        ws.send(JSON.stringify(data));
    }
    sseArray.forEach((sse) => {
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
