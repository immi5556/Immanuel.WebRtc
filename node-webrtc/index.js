const lib = require('nbind').init(__dirname).lib;

const instance = lib.Puff.getInstance(function (unicodeArray) {
    return String.fromCharCode.apply(null, unicodeArray);
}, function(length) { return new Buffer(length); });

module.exports = instance;