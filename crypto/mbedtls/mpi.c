#include "mpi.h"

void
mpi_init(mpi* num)
{
    mbedtls_mpi_init(num);
}

int
mpi_cmp(const mpi* a, const mpi* b)
{
    return mbedtls_mpi_cmp_mpi(a, b);
}
