#ifndef USYS_LOG_H_
#define USYS_LOG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"

// TODO fix debug switch (not catcing from cmake)
#if NDEBUG
#define usys_log_fn usys_log_
#else
#define usys_log_fn usys_log_
#endif

#define usys_log_ok(...) usys_log_fn(USYS_LOG_OK, __VA_ARGS__)
#define usys_log_note(...) usys_log_fn(USYS_LOG_NOTE, __VA_ARGS__)
#define usys_log_info(...) usys_log_fn(USYS_LOG_INFO, __VA_ARGS__)
#define usys_log_warn(...) usys_log_fn(USYS_LOG_WARN, __VA_ARGS__)
#define usys_log_err(...) usys_log_fn(USYS_LOG_ERRO, __VA_ARGS__)
#define usys_log(...) usys_log_info(__VA_ARGS__)

// clang-format off
#define USYS_LOG_RESET "\x1b[0m"
#define USYS_LOG_COLORS {   \
	"\x1b[32m",         \
	"\x1b[97m",         \
	"\x1b[2m\x1b[97m",  \
	"\x1b[93m",         \
	"\x1b[91m"}
// clang-format on

/// Indexes match above
typedef enum {
    USYS_LOG_OK = 0,   // Green
    USYS_LOG_NOTE = 1, // White - bright
    USYS_LOG_INFO = 2, // White - dim
    USYS_LOG_WARN = 3, // Yellow
    USYS_LOG_ERRO = 4  // Red
} USYS_LOG_LEVEL;

void usys_log_(USYS_LOG_LEVEL lvl, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
