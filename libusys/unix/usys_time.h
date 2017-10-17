#ifndef USYS_TIME_H_
#define USYS_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"

void usys_msleep(uint32_t ms);
int64_t usys_now();

#ifdef __cplusplus
}
#endif
#endif
