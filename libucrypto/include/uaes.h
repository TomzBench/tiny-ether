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

typedef mbedtls_aes_context uaes_ctx;

#define uaes_init_bin(ctx, key, keylen) uaes_init(ctx, keylen * 8, key)
#define uaes_init_128(ctx, key) uaes_init(ctx, 128, key)
#define uaes_init_256(ctx, key) uaes_init(ctx, 256, key)
int uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key);
void uaes_deinit(uaes_ctx** ctx);

#define uaes_crypt_ctr_128(k, iv, in, l, o)                                    \
    uaes_crypt_ctr(128, (k)->b, (iv)->b, in, l, o)
#define uaes_crypt_ctr_256(k, iv, in, l, o)                                    \
    uaes_crypt_ctr(256, (k)->b, (iv)->b, in, l, o)
int uaes_crypt_ctr(int keysz,
                   uint8_t* key,
                   uint8_t* iv,
                   const uint8_t* in,
                   size_t inlen,
                   uint8_t* out);
int uaes_crypt_ctr_op(uaes_ctx* ctx,
                      uint8_t* iv,
                      const uint8_t* in,
                      size_t inlen,
                      uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
