#include <napi.h>

#include <unistd.h>
#include <sys/wait.h>

#include <openssl/rand.h>

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

  // We don't have any entropy data here, so kindly ask OpenSSL impl to get
  // us some.
  if (!EVP_RAND_reseed(drbg, 0, NULL, 0, NULL, 0)) {
    Napi::TypeError::New(env, "EVP_RAND_reseed failed").ThrowAsJavaScriptException();
  }
}

void Reseed(const Napi::CallbackInfo& info) {
  Reseed_OpenSSL_RAND(info);
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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "reseed"), Napi::Function::New(env, Reseed));

  // internal services for tests
  exports.Set(Napi::String::New(env, "fork"), Napi::Function::New(env, Fork));
  exports.Set(Napi::String::New(env, "getpid"), Napi::Function::New(env, GetPid));
  exports.Set(Napi::String::New(env, "waitpid"), Napi::Function::New(env, WaitPid));

  return exports;
}

NODE_API_MODULE(reseed, Init)
