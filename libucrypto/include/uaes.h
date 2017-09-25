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

int uaes_init_ctr_bin(uaes_ctx* ctx, uint8_t* key, int keylen);
int uaes_init_ctr_128(uaes_ctx* ctx, uaes_ctr_128_key* key);
int uaes_init_ctr_256(uaes_ctx* ctx, uaes_ctr_256_key* key);
int uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key);

void uaes_deinit(uaes_ctx** ctx);

int uaes_crypt(int keysz,
               uint8_t* key,
               uint8_t* iv,
               const uint8_t* in,
               size_t inlen,
               uint8_t* out);
int uaes_crypt_ctr_128(uaes_ctr_128_key* key,
                       uaes_iv* iv,
                       const uint8_t* in,
                       size_t inlen,
                       uint8_t* out);
int uaes_crypt_ctr_256(uaes_ctr_256_key* key,
                       uaes_iv* iv,
                       const uint8_t* in,
                       size_t inlen,
                       uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
