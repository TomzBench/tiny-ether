#include "ukeccak256.h"
#include "urand.h"
#include "unonce.h"

int
unonce(uint8_t* out32)
{
    uint8_t r[32];
    urand(r, 32);
    return ukeccak256(r, 32, out32, 32);
}
