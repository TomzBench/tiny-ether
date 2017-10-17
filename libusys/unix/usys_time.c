#include "usys_time.h"
#include <time.h>

void
usys_msleep(uint32_t ms)
{
    usleep(ms * 1000);
}

int64_t
usys_now()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)((int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000);
}
