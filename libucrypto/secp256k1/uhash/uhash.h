#ifndef HMAC_H_
#define HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "secp256k1_sha256.h"

typedef secp256k1_hmac_sha256_t uhmac_sha256_ctx;
typedef secp256k1_sha256_t usha256_ctx;
typedef secp256k1_rfc6979_hmac_sha256_t urfc6979_hmac_sha256_ctx;

void usha256_init(usha256_ctx*);
void usha256_update(usha256_ctx*, const uint8_t*, size_t);
void usha256_finish(usha256_ctx*, uint8_t*);
void usha256_free(usha256_ctx*);
void usha256(const uint8_t* msg, size_t msglen, uint8_t* hmac);

void uhmac_sha256_init(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_update(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_finish(uhmac_sha256_ctx*, uint8_t*);
void uhmac_sha256_free(uhmac_sha256_ctx*);
void uhmac_sha256(const uint8_t* key,
                  size_t keylen,
                  const uint8_t* msg,
                  size_t msglen,
                  uint8_t* hmac);
#ifdef __cplusplus
}
#endif
#endif
