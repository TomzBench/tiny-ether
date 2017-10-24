#ifndef RLPX_DEVP2P_H_
#define RLPX_DEVP2P_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_protocol.h"

typedef enum {
    DEVP2P_ERRO = -0x01,
    DEVP2P_HELLO = 0x00,
    DEVP2P_DISCONNECT = 0x01,
    DEVP2P_PING = 0x02,
    DEVP2P_PONG = 0x03
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
    DEVP2P_DISCONNECT_QUITTING = 8,
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
    const rlpx_devp2p_protocol_settings* settings; /*!< user callbacks */
    char client[128];                              /*!< Hello packet client*/
    uint32_t listenport;                           /*!< */
    int64_t ping;                                  /*!< ping now() */
    uint32_t latency;                              /*!< now() - ping */
} rlpx_devp2p_protocol;

// Heap constructors
rlpx_devp2p_protocol* rlpx_devp2p_protocol_alloc(
    const rlpx_devp2p_protocol_settings* settings,
    void* ctx);
void rlpx_devp2p_protocol_free(rlpx_devp2p_protocol** self_p);

// Initializers/Deinitializers
void rlpx_devp2p_protocol_init(rlpx_devp2p_protocol* self,
                               const rlpx_devp2p_protocol_settings* settings,
                               void* ctx);
void rlpx_devp2p_protocol_deinit(rlpx_devp2p_protocol* self);

int rlpx_devp2p_protocol_write(rlpx_coder* x,
                               RLPX_DEVP2P_PROTOCOL_PACKET_TYPE type,
                               urlp* rlp,
                               uint8_t* out,
                               uint32_t* outlen);
int rlpx_devp2p_protocol_write_hello(rlpx_coder* x,
                                     uint32_t port,
                                     const uint8_t* id,
                                     uint8_t* out,
                                     uint32_t* l);
int rlpx_devp2p_protocol_write_disconnect(rlpx_coder* x,
                                          RLPX_DEVP2P_DISCONNECT_REASON reason,
                                          uint8_t* out,
                                          uint32_t* l);
int rlpx_devp2p_protocol_write_ping(rlpx_coder* x, uint8_t* out, uint32_t* l);
int rlpx_devp2p_protocol_write_pong(rlpx_coder* x, uint8_t* out, uint32_t* l);

static inline int
rlpx_devp2p_protocol_p2p_version(const urlp* rlp, uint32_t* out)
{
    return rlpx_rlp_to_u32(rlp, 0, out);
}

static inline int
rlpx_devp2p_protocol_client_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    return rlpx_rlp_to_mem(rlp, 1, ptr_p, l);
}

static inline int
rlpx_devp2p_protocol_capabilities(const urlp* rlp, const char* cap, uint32_t v)
{
    const urlp *seek, *caps = urlp_at(rlp, 2);
    uint32_t ver, sz, len = strlen(cap), n = caps ? urlp_siblings(caps) : 0;
    const char* mem;
    for (uint32_t i = 0; i < n; i++) {
        if (!(seek = urlp_at(caps, i))) continue;
        rlpx_rlp_to_mem(seek, 0, &mem, &sz);
        if ((sz == len) && (!(memcmp(mem, cap, len)))) {
            rlpx_rlp_to_u32(seek, 1, &ver);
            return (ver >= v) ? 0 : -1;
        }
    }

    return -1;
}

static inline int
rlpx_devp2p_protocol_listen_port(const urlp* rlp, uint32_t* port)
{
    return rlpx_rlp_to_u32(rlp, 3, port);
}

static inline int
rlpx_devp2p_protocol_node_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    return rlpx_rlp_to_mem(rlp, 4, ptr_p, l);
}

#ifdef __cplusplus
}
#endif
#endif
