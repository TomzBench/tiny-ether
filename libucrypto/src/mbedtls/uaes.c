#include "uaes.h"

int
uaes_init_ctr_128(uaes_ctx* ctx, uaes_ctr_128_key* key)
{
    return uaes_init(ctx, 128, key->b);
}

int
uaes_init_ctr_256(uaes_ctx* ctx, uaes_ctr_256_key* key)
{
    return uaes_init(ctx, 256, key->b);
}

int
uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key)
{
    mbedtls_aes_init(ctx);
    int err;
    err = (mbedtls_aes_setkey_enc(ctx, key, keysz)) ? -1 : 0;
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
uaes_crypt_ctr_128(uaes_ctr_128_key* key,
                   uaes_iv* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out)
{
    return uaes_crypt(128, key->b, iv->b, in, inlen, out);
}

int
uaes_crypt_ctr_256(uaes_ctr_256_key* key,
                   uaes_iv* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out)
{
    return uaes_crypt(256, key->b, iv->b, in, inlen, out);
}

int
uaes_crypt(int keysz,
           uint8_t* key,
           uint8_t* iv,
           const uint8_t* in,
           size_t inlen,
           uint8_t* out)
{

    int err = 0;
    uint8_t block[keysz / 8];
    size_t nc_off = 0;
    uaes_ctx ctx;
    err = uaes_init(&ctx, keysz, key);
    if (err) return err;
    err = mbedtls_aes_crypt_ctr(&ctx, inlen, &nc_off, iv, block, in, out);
    return err;
}
