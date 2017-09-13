#include "hmac.h"
#include <string.h>

void
ucrypto_hmac_sha256_init(ucrypto_hmac_sha256_ctx* ctx,
                         const uint8_t* key,
                         size_t klen)
{
}

void
ucrypto_hmac_sha256_update(ucrypto_hmac_sha256_ctx* ctx,
                           const uint8_t* b,
                           size_t l)
{
}

void
ucrypto_hmac_sha256_finish(ucrypto_hmac_sha256_ctx* ctx, uint8_t* hmac)
{
}

void
ucrypto_hmac_sha256_free(ucrypto_hmac_sha256_ctx* ctx)
{
}

void
ucrypto_hmac_sha256(const uint8_t* key,
                    size_t keylen,
                    const uint8_t* msg,
                    size_t msglen,
                    uint8_t* hmac)
{
}

void
ucrypto_sha256(const uint8_t* msg, size_t msglen, uint8_t* sha)
{
}

//
//
//
