#include "mpi.h"
#include "string.h"

int
ucrypto_mpi_xor(ucrypto_mpi* dst, ucrypto_mpi* src, uint8_t* bytes, size_t l)
{
    if (!(l <= dst->n)) return -1;
    if (!(l <= src->n)) return -1;
    for (uint32_t i = 0; i < l; i++) dst->p[i] ^= bytes[i];
    return 0;
}
