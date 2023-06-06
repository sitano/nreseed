#include "prng.h"

Napi::Object PRNG::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env,
                  "PRNG", {
                  InstanceMethod("Random",  &PRNG::Random),
                  InstanceMethod("SetSeed", &PRNG::SetSeed),
                  InstanceMethod("Reseed",  &PRNG::Reseed)});

  exports.Set("PRNG", func);

  return exports;
}

PRNG::PRNG(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PRNG>(info) {
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length == 0) {
    this->value_ = v8::base::RandomNumberGenerator(0);
    this->value_.Reseed();
  } else if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return;
  } else if (length != 1) {
    Napi::TypeError::New(env, "Expected 1 argument max").ThrowAsJavaScriptException();
    return;
  } else {
    Napi::Number value = info[0].As<Napi::Number>();
    int64_t seed = value.Int64Value();
    this->value_ = v8::base::RandomNumberGenerator(seed);
  }
}

Napi::Value PRNG::Random(const Napi::CallbackInfo& info) {
  const double num = this->value_.NextDouble();

  return Napi::Number::New(info.Env(), num);
}

void PRNG::SetSeed(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length <= 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return;
  }

  Napi::Number value = info[0].As<Napi::Number>();

  this->value_.SetSeed(value.Int64Value());
}

void PRNG::Reseed(const Napi::CallbackInfo& info) {
  this->value_.Reseed();
}

