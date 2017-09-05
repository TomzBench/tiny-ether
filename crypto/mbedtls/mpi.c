#include "mpi.h"

void
mpi_init(mpi* num)
{
    mbedtls_mpi_init(num);
}

void
mpi_free(mpi* num)
{
    mbedtls_mpi_free(num);
}

int
mpi_cmp(const mpi* a, const mpi* b)
{
    return mbedtls_mpi_cmp_mpi(a, b);
}
