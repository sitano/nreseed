// jshint esversion: 6

var seeder = require('../pkg/seeder');
var assert = require('assert');

let ticks = seeder.internal.ticks;

assert.notEqual(ticks(), ticks());
assert(ticks() > 1);
