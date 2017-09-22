#ifndef URLPX_CONFIG_COMMON_H_
#define URLPX_CONFIG_COMMON_H_

#ifdef URLPX_CONFIG_FILE
#include URLPX_CONFIG_FILE
#elif URLPX_CONFIG_LINUX_EMU
#include "rlpx_config_linux_emu.h"
#endif

#endif
