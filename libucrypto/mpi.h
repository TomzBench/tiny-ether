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

int ucrypto_mpi_xor(ucrypto_mpi* dst,
                    const ucrypto_mpi* src,
                    uint8_t* bytes,
                    size_t l);

#ifdef __cplusplus
}
#endif
#endif
