#include "uhash.h"
#include <string.h>

void
usha256_init(usha256_ctx* ctx)
{
    secp256k1_sha256_initialize(ctx);
}

void
usha256_update(usha256_ctx* ctx, const uint8_t* msg, size_t mlen)
{
    secp256k1_sha256_write(ctx, msg, mlen);
}

void
usha256_finish(usha256_ctx* ctx, uint8_t* sha)
{
    secp256k1_sha256_finalize(ctx, sha);
}

void
usha256_free(usha256_ctx* ctx)
{
    ((void)ctx);
}

void
usha256(const uint8_t* msg, size_t msglen, uint8_t* sha)
{
    usha256_ctx ctx;
    usha256_init(&ctx);
    usha256_update(&ctx, msg, msglen);
    usha256_finish(&ctx, sha);
    usha256_free(&ctx);
}

void
uhmac_sha256_init(uhmac_sha256_ctx* ctx, const uint8_t* key, size_t klen)
{
    secp256k1_hmac_sha256_initialize(ctx, key, klen);
}

void
uhmac_sha256_update(uhmac_sha256_ctx* ctx, const uint8_t* b, size_t l)
{
    secp256k1_hmac_sha256_write(ctx, b, l);
}

void
uhmac_sha256_finish(uhmac_sha256_ctx* ctx, uint8_t* hmac)
{
    secp256k1_hmac_sha256_finalize(ctx, hmac);
}

void
uhmac_sha256_free(uhmac_sha256_ctx* ctx)
{
    ((void)ctx);
}

void
uhmac_sha256(const uint8_t* key,
             size_t keylen,
             const uint8_t* msg,
             size_t msglen,
             uint8_t* hmac)
{
    uhmac_sha256_ctx ctx;
    uhmac_sha256_init(&ctx, key, keylen);
    uhmac_sha256_update(&ctx, msg, msglen);
    uhmac_sha256_finish(&ctx, hmac);
    uhmac_sha256_free(&ctx);
}

//
//
//
