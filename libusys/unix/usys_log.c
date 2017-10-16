#include "usys_log.h"

void
usys_log(USYS_LOG_LEVEL lvl, const char* fmt, ...)
{
    char buf[1024];
    int l;
    va_list ap;
    va_start(ap, fmt);
    l = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    printf("%s", buf);
}
