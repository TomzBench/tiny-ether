#include "keccak-tiny/keccak-tiny.h"

int
usha3(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen)
{
    return sha3_256(out, outlen, in, inlen);
}
