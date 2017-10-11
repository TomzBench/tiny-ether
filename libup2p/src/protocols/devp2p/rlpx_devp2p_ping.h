#ifndef RLPX_DEVP2P_PING_H_
#define RLPX_DEVP2P_PING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_protocol.h"

int rlpx_devp2p_write_ping(rlpx_coder* x, uint8_t* out, size_t* l);

#ifdef __cplusplus
}
#endif
#endif
