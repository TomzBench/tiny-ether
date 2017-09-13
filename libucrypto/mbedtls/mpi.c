#include "mpi.h"
#include <string.h>

#define ciL (sizeof(mbedtls_mpi_uint)) /* chars in limb  */
#define biL (ciL << 3)                 /* bits  in limb  */
#define biH (ciL << 2)                 /* half limb size */
#define BITS_TO_LIMBS(i) ((i) / biL + ((i) % biL != 0))
static int mpi_get_digit(mbedtls_mpi_uint* d, int radix, char c);

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
    int c;
    size_t i, j, k;
    char* p = buf;
    ((void)blen);
    ((void)olen);
    if (!(radix == 16)) return -1;

    if (X->s == -1) *p++ = '-';

    for (i = X->n, k = 0; i > 0; i--) {
        for (j = sizeof(mbedtls_mpi_uint); j > 0; j--) {
            c = (X->p[i - 1] >> ((j - 1) << 3)) & 0xFF;

            if (c == 0 && k == 0 && (i + j) != 2) continue;

            *(p++) = "0123456789ABCDEF"[c / 16];
            *(p++) = "0123456789ABCDEF"[c % 16];
            k = 1;
        }
    }
    *p++ = '\0';
    *olen = strlen(buf);
    return 0;
}

int
ubn_str(mbedtls_mpi* X, int radix, const char* s)
{
    size_t i, j, n, slen = strlen(s);
    if (!(radix == 16)) return -1;
    mbedtls_mpi_uint d;

    n = BITS_TO_LIMBS(slen << 2);
    mbedtls_mpi_grow(X, n);
    mbedtls_mpi_lset(X, 0);

    for (i = slen, j = 0; i > 0; i--, j++) {
        if (i == 1 && s[i - 1] == '-') {
            X->s = -1;
            break;
        }

        mpi_get_digit(&d, radix, s[i - 1]);
        X->p[j / (2 * sizeof(d))] |= d << ((j % (2 * sizeof(d))) << 2);
    }
    return 0;
}

static int
mpi_get_digit(mbedtls_mpi_uint* d, int radix, char c)
{
    *d = 255;

    if (c >= 0x30 && c <= 0x39) *d = c - 0x30;
    if (c >= 0x41 && c <= 0x46) *d = c - 0x37;
    if (c >= 0x61 && c <= 0x66) *d = c - 0x57;

    if (*d >= (mbedtls_mpi_uint)radix)
        return (MBEDTLS_ERR_MPI_INVALID_CHARACTER);

    return (0);
}
