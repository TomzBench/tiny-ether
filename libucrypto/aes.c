#include "aes.h"

int
ucrypto_aes_init(ucrypto_aes_ctx* ctx, ucrypto_aes_128_ctr_key* key)
{
    int err = 0;
    mbedtls_aes_init(ctx);
    err |= (mbedtls_aes_setkey_enc(ctx, key->b, 128)) ? -1 : 0;
    if (!(err == 0)) ucrypto_aes_deinit(&ctx);
    return err;
}

void
ucrypto_aes_deinit(ucrypto_aes_ctx** ctx_p)
{
    ucrypto_aes_ctx* ctx = *ctx_p;
    *ctx_p = NULL;
    mbedtls_aes_free(ctx);
}

int
ucrypto_aes_crypt(ucrypto_aes_128_ctr_key* key,
                  ucrypto_aes_iv* iv,
                  const uint8_t* in,
                  size_t inlen,
                  uint8_t* out)
{
    int err = 0;
    uint8_t block[16];
    size_t nc_off = 0;
    ucrypto_aes_ctx ctx;
    err = ucrypto_aes_init(&ctx, key);
    if (err) return err;
    err = mbedtls_aes_crypt_ctr(&ctx, inlen, &nc_off, iv->b, block, in, out);
    return err;
}
