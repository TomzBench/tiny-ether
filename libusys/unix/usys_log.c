#include "usys_log.h"
#include <stdarg.h>
#include <stdio.h>

void
usys_log(USYS_LOG_LEVEL lvl, const char* fmt, ...)
{
    ((void)lvl);
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}
