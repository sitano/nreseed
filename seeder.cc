#include <napi.h>
#include <openssl/rand.h>

// This one covers OpenSSL RAND/EVP/ossl_prov_drbg_generate,
// or reseeds Node.js's Crypto.getRandomBytes()/node::crypto::CSPRNG.
//
//     #0  0x0000000001b99354 in ossl_prov_drbg_generate ()
//     #1  0x0000000001a83969 in evp_rand_generate_locked ()
//     #2  0x0000000001a84859 in EVP_RAND_generate ()
//     #3  0x0000000001aeda44 in RAND_bytes ()
//     #4  0x0000000000d7ea96 in node::crypto::CSPRNG(void*, unsigned long) ()
void Reseed_OpenSSL_RAND(void) {
  EVP_RAND_CTX *drbg = RAND_get0_primary(NULL);
  if (drbg != NULL) {
    // We don't have any entropy data here, so kindly ask OpenSSL impl to get
    // us some.
    EVP_RAND_reseed(drbg, 0, NULL, 0, NULL, 0);
  }
  // FIXME: throw exception otherwise
}

void Reseed(const Napi::CallbackInfo& info) {
  Reseed_OpenSSL_RAND();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "reseed"), Napi::Function::New(env, Reseed));
  return exports;
}

NODE_API_MODULE(reseed, Init)
