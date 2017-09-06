#ifndef MPI_H_
#define MPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/bignum.h"

typedef mbedtls_mpi mpi; /*!< multi precesion integer */

void mpi_init(mpi*);
void mpi_free(mpi*);
int mpi_cmp(const mpi*, const mpi*);

#define mpi_write_binary(x, b, l) mbedtls_mpi_write_binary(x, b, l)
#define mpi_read_binary(x, b, l) mbedtls_mpi_read_binary(x, b, l)
#define mpi_write_string(x, r, b, l, o) mbedtls_mpi_write_string(x, r, b, l, o)
#define mpi_read_string(x, r, s) mbedtls_mpi_write_string(x, r, s)

#ifdef __cplusplus
}
#endif
#endif
