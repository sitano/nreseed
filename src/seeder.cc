#include <napi.h>
#include <node.h>

#include <array>
#include <chrono>
#include <string>
#include <sstream>

#include <sys/wait.h>
#include <unistd.h>

#include <openssl/rand.h>

#include "prng.h"

#include "utils/time.h"
#include "utils/bit_cast.h"

// This one covers OpenSSL RAND/EVP/ossl_prov_drbg_generate,
// or reseeds Node.js's Crypto.getRandomBytes()/node::crypto::CSPRNG.
//
//     #0  0x0000000001b99354 in ossl_prov_drbg_generate ()
//     #1  0x0000000001a83969 in evp_rand_generate_locked ()
//     #2  0x0000000001a84859 in EVP_RAND_generate ()
//     #3  0x0000000001aeda44 in RAND_bytes ()
//     #4  0x0000000000d7ea96 in node::crypto::CSPRNG(void*, unsigned long) ()
//
// however, see https://wiki.openssl.org/index.php/Random_fork-safety.
void Reseed_OpenSSL_RAND(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  EVP_RAND_CTX *drbg = RAND_get0_primary(NULL);
  if (drbg == NULL) {
    Napi::TypeError::New(env, "no primary EVP_RAND_CTX found").ThrowAsJavaScriptException();
    return;
  }

  int64_t addin = v8::base::NowFromSystemTime() << 24;
  addin ^= v8::base::TimeTicksNow();

  // We don't have any entropy data here, so kindly ask OpenSSL impl to get
  // us some. We could perform RAND_seed(getpid()) as it is advertised in
  // Random_fork-satefy, but in PID namespaces all processes will be having
  // the same pid_t with very high probability (around pid_t == 1).
  auto addin_buf = v8::base::bit_cast<std::array<unsigned char, 8>>(addin);
  if (!EVP_RAND_reseed(drbg, 0, NULL, 0, addin_buf.data(), 8)) {
    Napi::TypeError::New(env, "EVP_RAND_reseed failed").ThrowAsJavaScriptException();
  }
}

// I don't know how to call MathRandom::ResetContext() from here. As well as
// reset native_context.math_random_state() accessed from
// MathRandom::RefillCache(). MathRandom cache is resete during snapshot
// serialization but thats too expensive. We could hijack Refill function, but
// it's easier just to replace whole Math.random().
void Reseed_Math_RAND(const Napi::CallbackInfo& info) {
  auto now = std::chrono::high_resolution_clock::now();

  std::stringstream ss;
  ss << "--random_seed ";
  ss << now.time_since_epoch().count();

  const std::string s(ss.str());
  v8::V8::SetFlagsFromString(s.c_str());
}

void Reseed(const Napi::CallbackInfo& info) {
  Reseed_OpenSSL_RAND(info);
  Reseed_Math_RAND(info);
}


// fork(2) for test purposes only
Napi::Value Fork(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  auto pid = fork();
  if (pid < 0) {
    Napi::TypeError::New(env, "fork() failed").ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, pid);
}

// getpid(2) for test purposes only
Napi::Value GetPid(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), getpid());
}

// waitpid(2) for test purposes only
Napi::Value WaitPid(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    // expected waitpid(pid)
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Wrong 1st argument").ThrowAsJavaScriptException();
    return env.Null();
  }

  int wstatus = 0;
  const int pid = info[0].As<Napi::Number>().Int32Value();
  auto res = waitpid(pid, &wstatus, 0);
  if (res < 0) {
    Napi::TypeError::New(env, "waitpid() failed").ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, wstatus);
}


// for test purposes only
Napi::Value GetTimeTicksNow(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), v8::base::TimeTicksNow());
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "reseed"), Napi::Function::New(env, Reseed));

  PRNG::Init(env, exports);

  // internal services for tests
  exports.Set(Napi::String::New(env, "fork"), Napi::Function::New(env, Fork));
  exports.Set(Napi::String::New(env, "getpid"), Napi::Function::New(env, GetPid));
  exports.Set(Napi::String::New(env, "waitpid"), Napi::Function::New(env, WaitPid));
  exports.Set(Napi::String::New(env, "ticks"), Napi::Function::New(env, GetTimeTicksNow));

  return exports;
}

NODE_API_MODULE(nreseed, Init)
