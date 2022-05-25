function validBpm(bpm) {
    try {
        let final = parseInt(/^\d{1,3}$/.exec(bpm)[0], 10);
        return (isNaN(final))? null : final;
    } catch (e) {
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
        let final = parseInt(/^\d*$/.exec(number)[0], 10);
        return (isNaN(final))? null : final;
    } catch (e){
        return null;
    }
}

function validChannelName(channelName) {
    let final = (channelName === null) ? null : String(/^(?:[A-Za-z\d+/]{4})*(?:[A-Za-z\d+/]{2}==|[A-Za-z\d+/]{3}=|[A-Za-z\d+/]{4})$/.exec(channelName));
    return (channelName === final) ? final : null ;
}

function sendUpdates(ws, sseArray, data) {
    if (ws !== null) {
        ws.send(JSON.stringify(data));
    }
    sseArray.forEach((sse) => {
        sse.write(`data: ${JSON.stringify(data)}\n\n`);
    });
}

module.exports = { 
    validBpm,
    validBpmEnabled,
    validNumber,
    validChannelName,
    sendUpdates
};
