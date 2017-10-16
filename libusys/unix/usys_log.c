#include "usys_log.h"

void
usys_log(USYS_LOG_LEVEL lvl, const char* file, int line, const char* fmt, ...)
{
    ((void)lvl);
    va_list ap;
    va_start(ap, fmt);
    printf("FILE: %s LINE: %d ", file, line);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}
