#include "keccak-tiny/keccak-tiny.h"

/*
 * keccak-tiny modified to support ethereum
 */

int
ukeccak256(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen)
{
    if (!(outlen == 32)) return -1;
    return keccak_256(out, 32, in, inlen);
}
