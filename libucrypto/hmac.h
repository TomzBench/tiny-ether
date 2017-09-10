#ifndef HMAC_H_
#define HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/sha256.h"

typedef struct
{
    uint8_t pad[64];
    mbedtls_sha256_context sha;
} ucrypto_hmac_sha256;

void ucrypto_hmac_sha256_init(ucrypto_hmac_sha256*, const uint8_t*, size_t);
void ucrypto_hmac_sha256_update(ucrypto_hmac_sha256*, const uint8_t*, size_t);
void ucrypto_hmac_sha256_finish(ucrypto_hmac_sha256*, uint8_t*);
void ucrypto_hmac_sha256_free(ucrypto_hmac_sha256*);

#ifdef __cplusplus
}
#endif
#endif
