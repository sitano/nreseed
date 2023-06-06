#ifndef PRNG_H
#define PRNG_H

#include <napi.h>

#include "utils/random-number-generator.h"

// Object that provides original node.js PRNG (Math.random()).
class PRNG : public Napi::ObjectWrap<PRNG> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  PRNG(const Napi::CallbackInfo& info);

 private:
  Napi::Value Random(const Napi::CallbackInfo& info);
  void SetSeed(const Napi::CallbackInfo& info);
  void Reseed(const Napi::CallbackInfo& info);

  v8::base::RandomNumberGenerator value_;
};

#endif
