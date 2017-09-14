#include "uhmac.h"
#include <string.h>

void
uhmac_sha256_init(uhmac_sha256_ctx* ctx, const uint8_t* key, size_t klen)
{
}

void
uhmac_sha256_update(uhmac_sha256_ctx* ctx, const uint8_t* b, size_t l)
{
}

void
uhmac_sha256_finish(uhmac_sha256_ctx* ctx, uint8_t* hmac)
{
}

void
uhmac_sha256_free(uhmac_sha256_ctx* ctx)
{
}

void
uhmac_sha256(const uint8_t* key,
             size_t keylen,
             const uint8_t* msg,
             size_t msglen,
             uint8_t* hmac)
{
}

void
usha256(const uint8_t* msg, size_t msglen, uint8_t* sha)
{
}

//
//
//
