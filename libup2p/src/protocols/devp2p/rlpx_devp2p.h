#ifndef RLPX_DEVP2P_H_
#define RLPX_DEVP2P_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_devp2p_disconnect.h"
#include "rlpx_devp2p_hello.h"
#include "rlpx_devp2p_ping.h"
#include "rlpx_devp2p_pong.h"
#include "rlpx_protocol.h"

typedef enum {
    DEVP2P_HELLO = 0,
    DEVP2P_DISCONNECT = 1,
    DEVP2P_PING = 2,
    DEVP2P_PONG = 3
} RLPX_DEVP2P_PROTOCOL_PACKET_TYPE;

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

typedef struct
{
    rlpx_protocol_cb on_hello;
    rlpx_protocol_cb on_ping;
    rlpx_protocol_cb on_pong;
    rlpx_protocol_cb on_disconnect;
} rlpx_devp2p_protocol_settings;

typedef struct
{
    rlpx_protocol base;
    const rlpx_devp2p_protocol_settings* settings;
} rlpx_devp2p_protocol;

// Heap constructors
rlpx_devp2p_protocol* rlpx_devp2p_protocol_alloc(
    const rlpx_devp2p_protocol_settings* settings);
void rlpx_devp2p_protocol_free(rlpx_devp2p_protocol** self_p);

// Initializers/Deinitializers
void rlpx_devp2p_protocol_init(rlpx_devp2p_protocol* self,
                               const rlpx_devp2p_protocol_settings* settings);
void rlpx_devp2p_protocol_deinit(rlpx_devp2p_protocol* self);

#ifdef __cplusplus
}
#endif
#endif
