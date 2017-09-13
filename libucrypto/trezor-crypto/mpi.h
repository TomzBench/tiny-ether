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

typedef bignum256 ucrypto_mpi; /*!< multi precesion integer */

#define ucrypto_mpi_init(x) (void)
#define ucrypto_mpi_free(x) (void)
#define ucrypto_mpi_cmp(x, y) (bn_is_less(x, y) == 0)
#define ucrypto_mpi_write_binary(x, b, l) bn_write_be(x, b)
#define ucrypto_mpi_read_binary(x, b, l) bn_read_be(x, b)

int ucrypto_mpi_btoa(const uint8_t* b,
                     size_t blen,
                     int radix,
                     char* a,
                     size_t* olen);
int ucrypto_mpi_atob(int radix, const char* str, uint8_t* b, size_t* olen);

#ifdef __cplusplus
}
#endif
#endif
