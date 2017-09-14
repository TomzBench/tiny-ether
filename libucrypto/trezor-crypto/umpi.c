#include "umpi.h"
#include <string.h>

#define ciL (sizeof(uint32_t)) /* chars in limb  */
#define biL (ciL << 3)         /* bits  in limb  */
#define biH (ciL << 2)         /* half limb size */
#define BITS_TO_LIMBS(i) ((i) / biL + ((i) % biL != 0))

static int mpi_get_digit(uint32_t* d, int radix, char c);

int
ubn_btoa(const uint8_t* b, size_t blen, int radix, char* a, size_t* olen)
{
    return 0;
}

int
ubn_atob(int radix, const char* str, uint8_t* b, size_t* olen)
{
    return 0;
}

int
ubn_toa(ubn* X, int radix, char* buf, size_t blen, size_t* olen)
{
    return 0;
}

int
ubn_str(ubn* mpi, int radix, const char* s)
{
    uint8_t buff[64];
    size_t n, i, j, slen = strlen(s);
    uint32_t d;

    n = BITS_TO_LIMBS(slen << 2);
    for (i = slen, j = 0; i > 0; i--, j++) {
        if (i == 1 && s[i - 1] == '-') {
            // X->s = -1;
            break;
        }

        mpi_get_digit(&d, radix, s[i - 1]);
        buff[j / (2 * sizeof(d))] |= d << ((j % (2 * sizeof(d))) << 2);
    }
    bn_read_be(buff, mpi);
    return 0;
}

int
ubn_size(ubn* mpi)
{
    return 0;
}

static int
mpi_get_digit(uint32_t* d, int radix, char c)
{
    *d = 255;

    if (c >= 0x30 && c <= 0x39) *d = c - 0x30;
    if (c >= 0x41 && c <= 0x46) *d = c - 0x37;
    if (c >= 0x61 && c <= 0x66) *d = c - 0x57;

    if (*d >= (uint32_t)radix) return (-1);

    return (0);
}
