var seeder = require('../pkg/seeder');
var assert = require('assert');

console.log("PRNG.random() = " + (new seeder.PRNG()).random());
console.log("PRNG.random() = " + (new seeder.PRNG()).random());

const a = new seeder.PRNG();
const b = new seeder.PRNG(0);

function must_be_eq(l, r)  { assert.equal(l, r); return l; }
function must_be_not(l, r) { assert.notEqual(l, r); return l; }

console.log("PRNG() is well seeded: " + must_be_not(a.random(), (new seeder.PRNG()).random()));

console.log("a.random() = " + a.random());
console.log("a.random() = " + a.random());
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.39339363837962904));
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.39339364003432853));

b.setSeed(1);

console.log("b.setSeed(1)");
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.7662037068107435));
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.0562947746566671));

b.setSeed(0);

console.log("b.setSeed(0)");
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.39339363837962904));
console.log("b.random(fixed) = " + must_be_eq(b.random(), 0.39339364003432853));

a.reseed(); const c1 = a.random();
a.reseed(); const c2 = a.random();
console.log("a.reseed()");
console.log("a.random() + reseed() + a.random() = " + must_be_not(c1, c2) + " != " + c2);
