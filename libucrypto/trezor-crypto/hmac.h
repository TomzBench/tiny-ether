#ifndef HMAC_H_
#define HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "trezor-crypto/hmac.h"

typedef struct HMAC_SHA256_CTX ucrypto_hmac_sha256_ctx;

void ucrypto_hmac_sha256_init(ucrypto_hmac_sha256_ctx*, const uint8_t*, size_t);
void ucrypto_hmac_sha256_update(ucrypto_hmac_sha256_ctx*,
                                const uint8_t*,
                                size_t);
void ucrypto_hmac_sha256_finish(ucrypto_hmac_sha256_ctx*, uint8_t*);
void ucrypto_hmac_sha256_free(ucrypto_hmac_sha256_ctx*);
void ucrypto_hmac_sha256(const uint8_t* key,
                         size_t keylen,
                         const uint8_t* msg,
                         size_t msglen,
                         uint8_t* hmac);
void ucrypto_sha256(const uint8_t*, size_t, uint8_t*);

#ifdef __cplusplus
}
#endif
#endif
