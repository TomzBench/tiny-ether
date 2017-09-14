#include "umpi.h"
#include <string.h>

#define ciL (sizeof(mbedtls_mpi_uint)) /* chars in limb  */
#define biL (ciL << 3)                 /* bits  in limb  */
#define biH (ciL << 2)                 /* half limb size */
#define BITS_TO_LIMBS(i) ((i) / biL + ((i) % biL != 0))

int
ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen)
{
    int err = 0;
    ubn mpi;
    ubn_init(&mpi);
    err = ubn_bin(&mpi, b, blen);
    if (!err) err = ubn_toa(&mpi, radix, a, *olen, olen);
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
    err = ubn_str(&mpi, radix, str);
    if (!err) {
        l = mbedtls_mpi_size(&mpi);
        if (!(l <= *olen)) err = -1;
        *olen = l;
    }
    if (!err) err = ubn_tob(&mpi, b, l);
    ubn_free(&mpi);
    return err;
}

/**
 * @brief We hacked up the more proper mbedtls code for temporary
 *
 * @param X
 * @param radix
 * @param buf
 * @param blen
 * @param olen
 */
int
ubn_toa(ubn* X, int radix, char* buf, size_t blen, size_t* olen)
{
    return mbedtls_mpi_write_string(X, radix, buf, blen, olen);
}

int
ubn_str(mbedtls_mpi* X, int radix, const char* s)
{
    return mbedtls_mpi_read_string(X, radix, s);
}
