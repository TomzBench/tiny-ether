#ifndef RLPX_DEVP2P_DISCONNECT_H_
#define RLPX_DEVP2P_DISCONNECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_protocol.h"

typedef enum {
    DEVP2P_DISCONNECT_REQUEST = 0,
    DEVP2P_DISCONNECT_TCP_ERRO = 1,
    DEVP2P_DISCONNECT_BAD_PROTOCOL = 2,
    DEVP2P_DISCONNECT_USESLESS_PEER = 3,
    DEVP2P_DISCONNECT_TO_MANY_PEERS = 4,
    DEVP2P_DISCONNECT_ALREADY_CONNECTED = 5,
    DEVP2P_DISCONNECT_BAD_VERSION = 6,
    DEVP2P_DISCONNECT_NODE_ID_RECEIVED = 7,
    DEVP2P_DISCONNECT_CLIENT_QUITTING = 8,
    DEVP2P_DISCONNECT_UNEXPECTED_ID = 9,
    DEVP2P_DISCONNECT_SELF_CONNECT = 0x0a,
    DEVP2P_DISCONNECT_TIMEOUT = 0x0b,
    DEVP2P_DISCONNECT_OTHER = 0x10
} RLPX_DEVP2P_DISCONNECT_REASON;

int rlpx_devp2p_write_disconnect(rlpx_coder* x,
                                 RLPX_DEVP2P_DISCONNECT_REASON,
                                 uint8_t* out,
                                 size_t* l);

#ifdef __cplusplus
}
#endif
#endif
