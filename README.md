# nreseed

node.js reseeder for PRNGs for checkpointed processes.

Addon exposes original Node.js PRNG algorithm (xor128+ + murmur3(seed)).
It also reinstalls crypto api entropy.

# quick start

    $ npm install
    $ npm test

# usage

```javascript
  require('nreseed').reseed();
  Math.random();

  const a = new Uint32Array(1);
  crypto.getRandomValues(a);
```

or

```javascript
  const seeder = require('nreseed');

  const a = new seeder.PRNG();
  const b = new seeder.PRNG(0);

  a.random();
  b.random();

  b.setSeed(1);
  b.random();

  b.reseed();
  b.random();

  Math.random = function() {
    return b.random();
  };
```

# author

Ivan P. @JohnKoepi
