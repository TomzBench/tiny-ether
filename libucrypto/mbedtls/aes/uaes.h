#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/aes.h"
//#include "umpi.h"
//
// clang-format off
typedef struct { uint8_t b[256] ;} h2048;
typedef struct { uint8_t b[128] ;} h1024;
typedef struct { uint8_t b[65]  ;} h520;
typedef struct { uint8_t b[64]  ;} h512;
typedef struct { uint8_t b[32]  ;} h256;
typedef struct { uint8_t b[20]  ;} h160;
typedef struct { uint8_t b[16]  ;} h128;
typedef struct { uint8_t b[8]   ;} h64;
// clang-format on

typedef h128 uaes_128_ctr_key;
typedef h128 uaes_iv;

typedef mbedtls_aes_context uaes_ctx;

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
