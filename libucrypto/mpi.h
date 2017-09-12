#ifndef MPI_H_
#define MPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/bignum.h"

typedef mbedtls_mpi ucrypto_mpi; /*!< multi precesion integer */

#define ucrypto_mpi_init(x) mbedtls_mpi_init(x)
#define ucrypto_mpi_free(x) mbedtls_mpi_free(x)
#define ucrypto_mpi_cmp(x, y) mbedtls_mpi_cmp_mpi(x, y)
#define ucrypto_mpi_write_binary(x, b, l) mbedtls_mpi_write_binary(x, b, l)
#define ucrypto_mpi_read_binary(x, b, l) mbedtls_mpi_read_binary(x, b, l)
#define ucrypto_mpi_write_string(x, r, b, l, o)                                \
    mbedtls_mpi_write_string(x, r, b, l, o)
#define ucrypto_mpi_read_string(x, r, s) mbedtls_mpi_read_string(x, r, s)
#define ucrypto_mpi_size(x) mbedtls_mpi_size(x)

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
