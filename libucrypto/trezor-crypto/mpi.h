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

#define ubn_init(x) (void)
#define ubn_free(x) (void)
#define ubn_cmp(x, y) (bn_is_less(x, y) == 0)
#define ubn_tob(x, b, l) bn_write_be(x, b)
#define ubn_bin(x, b, l) bn_read_be(x, b)

int ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen);
int ubn_atob(int radix, const char* str, uint8_t* b, size_t* olen);

#ifdef __cplusplus
}
#endif
#endif
