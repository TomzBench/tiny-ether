/*
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

typedef struct
{
    async_io io;                 /*!< io context for network sys calls */
    rlpx_devp2p_protocol devp2p; /*!< hello/disconnect/ping/pong */
    rlpx_protocol* protocols[2]; /*!< protocol handlers */
    uecc_ctx ekey;               /*!< our epheremal key */
    uecc_ctx skey;               /*!< our static key */
    rlpx_handshake* hs;          /*!< temp context during handshake process */
    h256 nonce;                  /*!< local nonce */
    char node_id[65];            /*!< node id */
    uint32_t listen_port;        /*!< our listen port */
    uint64_t remote_version;     /*!< remote version from auth */
    h512 remote_node_id;         /*!< remote public address */
    h256 remote_nonce;           /*!< remote nonce */
    uecc_public_key remote_ekey; /*!< remote ephermeral pubkey */
    uecc_public_key remote_skey; /*!< remote static pubkey */
    rlpx_coder x;                /*!< igress/ingress */
} rlpx_channel;

// constructors
rlpx_channel* rlpx_ch_alloc(uecc_private_key* skey, uecc_private_key* ekey);
rlpx_channel* rlpx_ch_mock_alloc(async_io_settings*,
                                 uecc_private_key*,
                                 uecc_private_key*);
void rlpx_ch_free(rlpx_channel** ch_p);
int rlpx_ch_init(rlpx_channel*, uecc_private_key*, uecc_private_key*);
int rlpx_ch_mock_init(rlpx_channel*,
                      async_io_settings*,
                      uecc_private_key*,
                      uecc_private_key*);
void rlpx_ch_deinit(rlpx_channel* session);

// methods
void rlpx_ch_nonce(rlpx_channel* ch);
int rlpx_ch_connect(rlpx_channel* ch, const uecc_public_key* to);
int rlpx_ch_accept(rlpx_channel* ch, const uecc_public_key* from);
int rlpx_ch_recv(rlpx_channel* ch, const uint8_t* d, size_t l);
int rlpx_ch_recv_auth(rlpx_channel*, const uint8_t*, size_t l);
int rlpx_ch_recv_ack(rlpx_channel* ch, const uint8_t*, size_t l);
int rlpx_ch_write_hello(rlpx_channel* ch, uint8_t* out, size_t* l);
int rlpx_ch_write_disconnect(rlpx_channel* ch,
                             RLPX_DEVP2P_DISCONNECT_REASON,
                             uint8_t* out,
                             size_t* l);
int rlpx_ch_write_ping(rlpx_channel* ch, uint8_t* out, size_t* l);
int rlpx_ch_write_pong(rlpx_channel* ch, uint8_t* out, size_t* l);

#ifdef __cplusplus
}
#endif
#endif
