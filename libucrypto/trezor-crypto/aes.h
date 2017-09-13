#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mpi.h"
#include "trezor-crypto/aes/aes.h"

typedef h128 uaes_128_ctr_key;
typedef h128 uaes_iv;

typedef aes_encrypt_ctx uaes_ctx;

int uaes_init(uaes_ctx* ctx, uaes_128_ctr_key* key);
void uaes_deinit(uaes_ctx** ctx);

int uaes_crypt(uaes_128_ctr_key* key,
               uaes_iv* iv,
               const uint8_t* in,
               size_t inlen,
               uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
