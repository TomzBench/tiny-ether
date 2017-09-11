#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/aes.h"

typedef uint8_t ucrypto_aes_128_ctr_key[16];
typedef uint8_t ucrypto_aes_iv[16];
typedef mbedtls_aes_context ucrypto_aes_ctx;

int ucrypto_aes_init(ucrypto_aes_ctx* ctx, ucrypto_aes_128_ctr_key* key);
void ucrypto_aes_deinit(ucrypto_aes_ctx** ctx);

int ucrypto_aes_crypt(ucrypto_aes_128_ctr_key* key,
                      ucrypto_aes_iv* iv,
                      const uint8_t* in,
                      size_t inlen,
                      uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
