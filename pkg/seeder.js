// jshint esversion: 6

var addon = require('bindings')('nreseed');

module.exports.PRNG = addon.PRNG;
module.exports.internal = addon;

module.exports.reseed = function() {
  addon.reseed();

  var prng = new addon.PRNG();
  Math.random = function() {
    return prng.random();
  };
};
