#include "hmac.h"
#include <string.h>

void
ucrypto_hmac_sha256_init(ucrypto_hmac_sha256_ctx* ctx,
                         const uint8_t* key,
                         size_t klen)
{
    static uint8_t i_key_pad[64];
    memset(i_key_pad, 0, 64);
    memset(ctx, 0, sizeof(ucrypto_hmac_sha256_ctx));
    if (klen > 64) {
        ucrypto_sha256(key, klen, i_key_pad);
    } else {
        memcpy(i_key_pad, key, klen);
    }
    for (int i = 0; i < 64; i++) {
        ctx->pad[i] = i_key_pad[i] ^ 0x5c;
        i_key_pad[i] ^= 0x36;
    }
    mbedtls_sha256_init(&ctx->sha);
    mbedtls_sha256_starts(&ctx->sha, 0);
    mbedtls_sha256_update(&ctx->sha, i_key_pad, 64);
    memset(i_key_pad, 0, sizeof(i_key_pad));
}

void
ucrypto_hmac_sha256_update(ucrypto_hmac_sha256_ctx* ctx,
                           const uint8_t* b,
                           size_t l)
{
    mbedtls_sha256_update(&ctx->sha, b, l);
}

void
ucrypto_hmac_sha256_finish(ucrypto_hmac_sha256_ctx* ctx, uint8_t* hmac)
{
    mbedtls_sha256_finish(&ctx->sha, hmac);
    mbedtls_sha256_init(&ctx->sha);
    mbedtls_sha256_starts(&ctx->sha, 0);
    mbedtls_sha256_update(&ctx->sha, ctx->pad, 64);
    mbedtls_sha256_update(&ctx->sha, hmac, 32);
    mbedtls_sha256_finish(&ctx->sha, hmac);
}

void
ucrypto_hmac_sha256_free(ucrypto_hmac_sha256_ctx* ctx)
{
    mbedtls_sha256_free(&ctx->sha);
}

void
ucrypto_hmac_sha256(const uint8_t* key,
                    size_t keylen,
                    const uint8_t* msg,
                    size_t msglen,
                    uint8_t* hmac)
{
    ucrypto_hmac_sha256_ctx ctx;
    ucrypto_hmac_sha256_init(&ctx, key, keylen);
    ucrypto_hmac_sha256_update(&ctx, msg, msglen);
    ucrypto_hmac_sha256_finish(&ctx, hmac);
    ucrypto_hmac_sha256_free(&ctx);
}

void
ucrypto_sha256(const uint8_t* msg, size_t msglen, uint8_t* sha)
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, msg, msglen);
    mbedtls_sha256_finish(&ctx, sha);
    mbedtls_sha256_free(&ctx);
}

//
//
//
