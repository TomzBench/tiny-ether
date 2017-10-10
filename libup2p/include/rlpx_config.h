#ifndef URLPX_CONFIG_COMMON_H_
#define URLPX_CONFIG_COMMON_H_

// Include a user config file or use a default
#ifdef URLPX_CONFIG_FILE
#include URLPX_CONFIG_FILE
#elif URLPX_CONFIG_LINUX_EMU
#include "rlpx_config_linux_emu.h"
#endif

// P2P client name
#define RLPX_CLIENT_ID_STR "tiny-ether"
#define RLPX_CLIENT_ID_LEN (sizeof(RLPX_CLIENT_ID_STR) - 1)
#define RLPX_VERSION_P2P 4

#endif
