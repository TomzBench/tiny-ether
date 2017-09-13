#ifndef MPI_H_
#define MPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/bignum.h"

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

typedef mbedtls_mpi ubn; /*!< multi precesion integer */

#define ubn_init(x) mbedtls_mpi_init(x)
#define ubn_free(x) mbedtls_mpi_free(x)
#define ubn_cmp(x, y) mbedtls_mpi_cmp_mpi(x, y)
#define ubn_write_binary(x, b, l) mbedtls_mpi_write_binary(x, b, l)
#define ubn_read_binary(x, b, l) mbedtls_mpi_read_binary(x, b, l)
#define ubn_write_string(x, r, b, l, o) mbedtls_mpi_write_string(x, r, b, l, o)
#define ubn_read_string(x, r, s) mbedtls_mpi_read_string(x, r, s)
#define ubn_size(x) mbedtls_mpi_size(x)

int ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen);
int ubn_atob(int radix, const char* str, uint8_t* b, size_t* olen);

#ifdef __cplusplus
}
#endif
#endif