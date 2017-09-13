#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "trezor-crypto/aes/aes.h"
#include "mpi.h"

typedef h128 ucrypto_aes_128_ctr_key;
typedef h128 ucrypto_aes_iv;

typedef aes_encrypt_ctx ucrypto_aes_ctx;

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
