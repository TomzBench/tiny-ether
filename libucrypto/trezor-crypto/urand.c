#include "urand.h"

int
urand(void* os, uint8_t* b, size_t l)
{
    ((void)os);
    random_buffer(b, l);
    return 0;
}
