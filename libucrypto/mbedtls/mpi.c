#include "mpi.h"

int
ucrypto_mpi_btoa(const uint8_t* b,
                 size_t blen,
                 int radix,
                 char* a,
                 size_t* olen)
{
    int err = 0;
    ucrypto_mpi mpi;
    ucrypto_mpi_init(&mpi);
    err = ucrypto_mpi_read_binary(&mpi, b, blen);
    if (!err) err = ucrypto_mpi_write_string(&mpi, radix, a, *olen, olen);
    ucrypto_mpi_free(&mpi);
    return err;
}

int
ucrypto_mpi_atob(int radix, const char* str, uint8_t* b, size_t* olen)
{
    int err = 0;
    size_t l;
    ucrypto_mpi mpi;
    ucrypto_mpi_init(&mpi);
    err = ucrypto_mpi_read_string(&mpi, radix, str);
    if (!err) {
        l = mbedtls_mpi_size(&mpi);
        if (!(l <= *olen)) err = -1;
        *olen = l;
    }
    if (!err) err = ucrypto_mpi_write_binary(&mpi, b, l);
    ucrypto_mpi_free(&mpi);
    return err;
}
