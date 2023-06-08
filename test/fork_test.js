// jshint esversion: 6

var seeder = require('../pkg/seeder');
var assert = require('assert');
var crypto;

try {
  crypto = require('node:crypto');
} catch (err) {
  console.error('crypto support is disabled!');
}

let fork = seeder.internal.fork;
let getpid = seeder.internal.getpid;
let waitpid = seeder.internal.waitpid;
let getRandomValues = function(a) {
  if (crypto) {
    return crypto.getRandomValues(a);
  }

  return undefined;
}

function log(...args) {
  let s = `#${getpid()} `;
  for (const arg of args) {
    s += arg;
  }
  console.log(s);
}

log("start");

Math.random();

// load some initial entropy
const a = new Uint32Array(1);
getRandomValues(a);

function test() {
  log("Math.random() = ", Math.random());
  const a = new Uint32Array(1);
  getRandomValues(a);
  log("crypto.getRandomValues() = ", a);
}

function test1() {
  test();
  seeder.reseed();
  log("reseeded");
  test();
}

// however, see https://wiki.openssl.org/index.php/Random_fork-safety.
const pid = fork();

if (pid == 0) {
  test1();
  log("child done");
  process.exit(0);
}

if (waitpid(pid) != 0) {
  assert("waitpid didn't do well");
}

test1();
log("parent done");
