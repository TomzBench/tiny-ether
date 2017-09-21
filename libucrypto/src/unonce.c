#include "urand.h"
#include "usha3.h"

int
unonce(uint8_t* out32)
{
    uint8_t r[32];
    urand(r, 32);
    return usha3(r, 32, out32, 32);
}
