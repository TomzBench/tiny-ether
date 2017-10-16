#ifndef USYS_LOG_H_
#define USYS_LOG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"

typedef enum { //
    USYS_LOG_NONE = 0,
    USYS_LOG_ERRO = 1,
    USYS_LOG_WARN = 2,
    USYS_LOG_ALL = 3
} USYS_LOG_LEVEL;

// TODO - check log level compile and void log calls
#define usys_log_warn(x) usys_log(USYS_LOG_WARN, (x))

void usys_log(USYS_LOG_LEVEL lvl, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
