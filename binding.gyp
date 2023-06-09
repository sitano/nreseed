{
  "targets": [
    {
      "target_name": "nreseed",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "src/seeder.cc",
        "src/prng.cc",
        "src/utils/time.cc",
        "src/utils/random-number-generator.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
