#include "mpi.h"

int
ucrypto_mpi_btoa(const uint8_t* b,
                 size_t blen,
                 int radix,
                 char* a,
                 size_t* olen)
{
    return 0;
}

int
ucrypto_mpi_atob(int radix, const char* str, uint8_t* b, size_t* olen)
{
    return 0;
}

int
ucrypto_mpi_write_string(ucrypto_mpi* mpi, int radix, uint8_t* b, size_t* olen)
{
    return 0;
}

int
ucrypto_mpi_read_string(ucrypto_mpi* mpi, int radix, const char* str)
{
    return 0;
}

int
ucrypto_mpi_size(ucrypto_mpi* mpi)
{
    return 0;
}
