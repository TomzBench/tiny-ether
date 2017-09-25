#include "keccak-tiny/keccak-tiny.h"

int
usha3(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen)
{
    if (!(outlen == 32)) return -1;
    return sha3_256(out, 32, in, inlen);
}
