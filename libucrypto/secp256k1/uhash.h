#ifndef UHASH_H_
#define UHASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hash.h"
// secp256k1_sha256_t;

typedef secp256k1_sha256_t uhmac_sha256_ctx;

void uhmac_sha256_init(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_update(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_finish(uhmac_sha256_ctx*, uint8_t*);
void uhmac_sha256_free(uhmac_sha256_ctx*);
void uhmac_sha256(const uint8_t* key,
                  size_t keylen,
                  const uint8_t* msg,
                  size_t msglen,
                  uint8_t* hmac);
void usha256(const uint8_t*, size_t, uint8_t*);

#ifdef __cplusplus
}
#endif
#endif
