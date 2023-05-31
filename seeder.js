var addon = require('bindings')('nreseed');

module.exports.reseed = addon.reseed;
module.exports.internal = addon;
