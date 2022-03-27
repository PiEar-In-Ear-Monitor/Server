function channelNameValidateId(id) {
    if (id != null && /[0-9]*/.test(id)) {
        return parseInt(id);
    }
    return false;
}

module.exports = { 
    channelNameValidateId
};
