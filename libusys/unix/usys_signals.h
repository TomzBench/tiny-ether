#ifndef USYS_SIGNALS_H_
#define USYS_SIGNALS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"

void usys_install_signal_handlers();
int usys_running();
void usys_shutdown();

#ifdef __cplusplus
}
#endif
#endif
