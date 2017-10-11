#ifndef RLPX_DEVP2P_H_
#define RLPX_DEVP2P_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_devp2p_disconnect.h"
#include "rlpx_devp2p_hello.h"
#include "rlpx_devp2p_ping.h"
#include "rlpx_devp2p_pong.h"

typedef enum {
    DEVP2P_HELLO = 0x80,
    DEVP2P_DISCONNECT = 0x01,
    DEVP2P_PING = 0x02,
    DEVP2P_PONG = 0x03
} RLPX_DEVP2P_PROTOCOL_PACKET_TYPE;

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
