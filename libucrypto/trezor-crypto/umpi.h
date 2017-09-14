#ifndef MPI_H_
#define MPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "trezor-crypto/bignum.h"

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

typedef bignum256 ubn; /*!< multi precesion integer */

#define ubn_init(x) ((void)(x))
#define ubn_free(x) ((void)(x))
#define ubn_cmp(x, y) (bn_is_less(x, y) == 0)
#define ubn_tob(x, b, l) bn_write_be(x, b)

int ubn_size(ubn*);
int ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen);
int ubn_atob(int radix, const char* str, uint8_t* b, size_t* olen);
int ubn_toa(ubn* X, int radix, char* buf, size_t blen, size_t* olen);
int ubn_str(ubn* X, int radix, const char* s);
int ubn_bin(ubn* X, uint8_t *b, size_t l);

#ifdef __cplusplus
}
#endif
#endif
