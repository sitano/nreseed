var addon = require('bindings')('nreseed');

module.exports.reseed = addon.reseed;
module.exports.PRNG = addon.PRNG;
module.exports.internal = addon;
