#ifndef MPI_H_
#define MPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/bignum.h"

typedef mbedtls_mpi mpi; /*!< multi precesion integer */

#define mpi_init(x) mbedtls_mpi_init(x)
#define mpi_free(x) mbedtls_mpi_free(x)
#define mpi_cmp(x, y) mbedtls_mpi_cmp_mpi(x, y)
#define mpi_write_binary(x, b, l) mbedtls_mpi_write_binary(x, b, l)
#define mpi_read_binary(x, b, l) mbedtls_mpi_read_binary(x, b, l)
#define mpi_write_string(x, r, b, l, o) mbedtls_mpi_write_string(x, r, b, l, o)
#define mpi_read_string(x, r, s) mbedtls_mpi_read_string(x, r, s)

int mpi_xor(mpi* dst, mpi* src, uint8_t* bytes, size_t l);

#ifdef __cplusplus
}
#endif
#endif
