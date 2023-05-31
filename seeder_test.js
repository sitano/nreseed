var seeder = require('./seeder');

console.log(1);
console.log(Math.random());
const a = new Uint32Array(1);
crypto.getRandomValues(a);
console.log(a);

seeder.reseed();
console.log("reseeded");

crypto.getRandomValues(a);
console.log(Math.random());
console.log(a);
