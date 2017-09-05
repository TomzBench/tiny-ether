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

#ifdef __cplusplus
}
#endif
#endif
