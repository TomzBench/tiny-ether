#include "aes.h"

int
uaes_init(uaes_ctx* ctx, uaes_128_ctr_key* key)
{
    int err = 0;
    mbedtls_aes_init(ctx);
    err |= (mbedtls_aes_setkey_enc(ctx, key->b, 128)) ? -1 : 0;
    if (!(err == 0)) uaes_deinit(&ctx);
    return err;
}

void
uaes_deinit(uaes_ctx** ctx_p)
{
    uaes_ctx* ctx = *ctx_p;
    *ctx_p = NULL;
    mbedtls_aes_free(ctx);
}

int
uaes_crypt(uaes_128_ctr_key* key,
           uaes_iv* iv,
           const uint8_t* in,
           size_t inlen,
           uint8_t* out)
{
    int err = 0;
    uint8_t block[16];
    size_t nc_off = 0;
    uaes_ctx ctx;
    err = uaes_init(&ctx, key);
    if (err) return err;
    err = mbedtls_aes_crypt_ctr(&ctx, inlen, &nc_off, iv->b, block, in, out);
    return err;
}
