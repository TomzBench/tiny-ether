#include "uaes.h"

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
uaes_crypt_ctr(int keysz,
               uint8_t* key,
               uint8_t* iv,
               const uint8_t* in,
               size_t inlen,
               uint8_t* out)
{

    int err = 0;
    uaes_ctx ctx;
    uint8_t block[16];
    size_t nc_off = 0;
    err = uaes_init(&ctx, keysz, key);
    if (err) return err;
    err = mbedtls_aes_crypt_ctr(&ctx, inlen, &nc_off, iv, block, in, out);
    return err;
}

int
uaes_crypt_ctr_op(uaes_ctx* ctx,
                  uint8_t* iv,
                  const uint8_t* in,
                  size_t inlen,
                  uint8_t* out)
{
    int err = 0;
    uint8_t block[16];
    size_t nc_off = 0;
    err = mbedtls_aes_crypt_ctr(ctx, inlen, &nc_off, iv, block, in, out);
    return err;
}

int
uaes_crypt_ecb_enc(uaes_ctx* ctx, const uint8_t* in, uint8_t* out)
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, in, out);
}

int
uaes_crypt_ecb_dec(uaes_ctx* ctx, const uint8_t* in, uint8_t* out)
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT, in, out);
}

//
//
//
