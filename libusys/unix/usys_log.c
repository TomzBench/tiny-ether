#include "usys_log.h"

void
usys_log_(USYS_LOG_LEVEL lvl, const char* fmt, ...)
{
    char* usys_colors[] = USYS_LOG_COLORS;
    va_list ap;

    // Switch font color
    printf("%s", usys_colors[lvl]);

    // print user
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    // newline
    printf("\n" USYS_LOG_RESET);
}
