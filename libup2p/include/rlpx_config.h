#ifndef URLPX_CONFIG_COMMON_H_
#define URLPX_CONFIG_COMMON_H_

#ifdef URLPX_CONFIG_FILE
#include URLPX_CONFIG_FILE
#elif URLPX_CONFIG_LINUX_EMU
#include "rlpx_config_linux_emu.h"
#endif

#define RLPX_CLIENT_ID_STR "tiny-ether"
#define RLPX_CLIENT_ID_LEN (sizeof(RLPX_CLIENT_ID_STR) - 1)
#define RLPX_VERSION_P2P 4

#endif
