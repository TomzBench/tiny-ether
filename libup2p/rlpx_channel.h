/**
 * @file rlpx_internal.h
 *
 * @brief
 */
#ifndef RLPX_CHANNEL_H_
#define RLPX_CHANNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "async_io.h"
#include "rlpx_config.h"
#include "rlpx_devp2p.h"
#include "rlpx_handshake.h"
#include "rlpx_node.h"

typedef struct
{
    async_io io;                 /*!< io context for network sys calls */
    rlpx_devp2p_protocol devp2p; /*!< hello/disconnect/ping/pong */
    rlpx_protocol* protocols[2]; /*!< protocol handlers */
    uecc_ctx* skey;              /*!< TODO make const - our static key ref*/
    uecc_ctx ekey;               /*!< our epheremal key */
    rlpx_coder x;                /*!< igress/ingress */
    h256 nonce;                  /*!< local nonce */
    rlpx_node node;              /*!< remote node info */
    rlpx_handshake* hs;          /*!< temp context during handshake process */
    int ready;                   /*!< handshake complete */
    int shutdown;                /*!< shutting down */
    uint8_t node_id[65];         /*!< node id */
    uint32_t listen_port;        /*!< our listen port */
} rlpx_channel;

// constructors
rlpx_channel* rlpx_ch_alloc(uecc_ctx* skey);
rlpx_channel* rlpx_ch_mock_alloc(async_io_settings*, uecc_ctx*);
void rlpx_ch_free(rlpx_channel** ch_p);
int rlpx_ch_init(rlpx_channel*, uecc_ctx*);
int rlpx_ch_mock_init(rlpx_channel*, async_io_settings*, uecc_ctx*);
void rlpx_ch_deinit(rlpx_channel* session);

// methods
void rlpx_ch_nonce(rlpx_channel* ch);
int rlpx_ch_poll(rlpx_channel** ch, uint32_t count, uint32_t ms);
int rlpx_ch_connect(rlpx_channel* ch,
                    const uecc_public_key* to,
                    const char* host,
                    uint32_t tcp);
int rlpx_ch_connect_enode(rlpx_channel* ch, const char* enode);
int rlpx_ch_connect_node(rlpx_channel* ch, const rlpx_node* node);
int rlpx_ch_accept(rlpx_channel* ch, const uecc_public_key* from);
int rlpx_ch_send_auth(rlpx_channel* ch);
int rlpx_ch_send_hello(rlpx_channel* ch);
int rlpx_ch_send_disconnect(rlpx_channel* ch, RLPX_DEVP2P_DISCONNECT_REASON);
int rlpx_ch_send_ping(rlpx_channel* ch);
int rlpx_ch_send_pong(rlpx_channel* ch);
int rlpx_ch_recv(rlpx_channel* ch, const uint8_t* d, size_t l);
int rlpx_ch_recv_auth(rlpx_channel*, const uint8_t*, size_t l);
int rlpx_ch_recv_ack(rlpx_channel* ch, const uint8_t*, size_t l);

static inline int
rlpx_ch_is_connected(rlpx_channel* ch)
{
    return async_io_has_sock(&ch->io);
}

static inline int
rlpx_ch_is_ready(rlpx_channel* ch)
{
    return ch->ready;
}

static inline int
rlpx_ch_is_shutdown(rlpx_channel* ch)
{
    return ch->shutdown;
}

#ifdef __cplusplus
}
#endif
#endif
