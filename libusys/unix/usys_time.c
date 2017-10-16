#include "usys_time.h"

void
usys_msleep(uint32_t ms)
{
    usleep(ms * 1000);
}
