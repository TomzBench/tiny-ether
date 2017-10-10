#ifndef USYS_CONFIG_H_
#define USYS_CONFIG_H_

// Include a user config file or use a default
#ifdef USYS_CONFIG_FILE
#include USYS_CONFIG_FILE
#elif URLPX_CONFIG_LINUX_EMU
#include "usys_config_unix.h"
#endif

#endif
