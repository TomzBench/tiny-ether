#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/aes.h"

// clang-format off
typedef struct{uint8_t b[16];} uaes_ctr_128_key;
typedef struct{uint8_t b[32];} uaes_ctr_256_key;
typedef struct{uint8_t b[16];} uaes_iv;
// clang-format on

// typedef mbedtls_aes_context uaes_ctx;
typedef struct
{
    mbedtls_aes_context ctx;
    uint8_t iv[16];
} uaes_ctx;

int uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key);
void uaes_deinit(uaes_ctx** ctx);
int uaes_crypt_ctr(int keysz,
                   uint8_t* key,
                   uint8_t* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out);
int uaes_crypt_ctr_update(uaes_ctx* ctx,
                          const uint8_t* in,
                          size_t inlen,
                          uint8_t* out);
int uaes_crypt_ctr_op(uaes_ctx* ctx,
                      uint8_t* iv,
                      const uint8_t* in,
                      size_t inlen,
                      uint8_t* out);

/**
 * @brief Inline aliaes api wrapper
 */

static inline int
uaes_init_bin(uaes_ctx* ctx, uint8_t* key, size_t keylen)
{
    return uaes_init(ctx, keylen * 8, key);
}

static inline int
uaes_init_128(uaes_ctx* ctx, uint8_t* key)
{
    return uaes_init(ctx, 128, key);
}

static inline int
uaes_init_256(uaes_ctx* ctx, uint8_t* key)
{
    return uaes_init(ctx, 256, key);
}

static inline int
uaes_crypt_ctr_128(uaes_ctr_128_key* key,
                   uaes_iv* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out)
{
    return uaes_crypt_ctr(128, key->b, iv->b, in, inlen, out);
}

static inline int
uaes_crypt_ctr_256(uaes_ctr_128_key* key,
                   uaes_iv* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out)
{
    return uaes_crypt_ctr(256, key->b, iv->b, in, inlen, out);
}

int uaes_crypt_ecb_enc(uaes_ctx* ctx, const uint8_t* in, uint8_t* out);
int uaes_crypt_ecb_dec(uaes_ctx* ctx, const uint8_t* in, uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
