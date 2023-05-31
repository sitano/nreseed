var seeder = require('./seeder');
var assert = require('assert');

const fork = seeder.internal.fork;
const getpid = seeder.internal.getpid;
const waitpid = seeder.internal.waitpid;

console.log("start");

Math.random();
// load some initial entropy
const a = new Uint32Array(1);
crypto.getRandomValues(a);

function test() {
  console.log(`#${getpid()} Math.random() = ${Math.random()}`);
  const a = new Uint32Array(1);
  crypto.getRandomValues(a);
  console.log(`#${getpid()} crypto.getRandomValues() = ${a}`);
}

function test1() {
  test();
  seeder.reseed();
  console.log(`#${getpid()} reseeded`);
  test();
}

// however, see https://wiki.openssl.org/index.php/Random_fork-safety.
const pid = fork();

if (pid == 0) {
  test1();
  console.log(`#${getpid()} child done`);
  process.exit(0);
}

if (waitpid(pid) != 0) {
  assert("waitpid didn't do well");
}

test1();
console.log(`#${getpid()} parent done`);
