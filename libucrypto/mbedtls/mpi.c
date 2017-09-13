#include "mpi.h"

int
ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen)
{
    int err = 0;
    ubn mpi;
    ubn_init(&mpi);
    err = ubn_read_binary(&mpi, b, blen);
    if (!err) err = ubn_write_string(&mpi, radix, a, *olen, olen);
    ubn_free(&mpi);
    return err;
}

int
ubn_atob(int radix, const char* str, uint8_t* b, size_t* olen)
{
    int err = 0;
    size_t l;
    ubn mpi;
    ubn_init(&mpi);
    err = ubn_read_string(&mpi, radix, str);
    if (!err) {
        l = mbedtls_mpi_size(&mpi);
        if (!(l <= *olen)) err = -1;
        *olen = l;
    }
    if (!err) err = ubn_write_binary(&mpi, b, l);
    ubn_free(&mpi);
    return err;
}
