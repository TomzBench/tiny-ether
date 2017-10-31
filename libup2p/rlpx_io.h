// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

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
    const uint32_t* listen_port; /*!< our listen port */
} rlpx_channel;

// constructors
rlpx_channel* rlpx_io_alloc(uecc_ctx* skey, const uint32_t*);
rlpx_channel*
rlpx_io_mock_alloc(async_io_settings*, uecc_ctx*, const uint32_t*);
void rlpx_io_free(rlpx_channel** ch_p);
int rlpx_io_init(rlpx_channel*, uecc_ctx*, const uint32_t*);
int rlpx_io_mock_init(
    rlpx_channel*,
    async_io_settings*,
    uecc_ctx*,
    const uint32_t*);
void rlpx_io_deinit(rlpx_channel* session);

// methods
void rlpx_io_nonce(rlpx_channel* ch);
int rlpx_io_poll(rlpx_channel** ch, uint32_t count, uint32_t ms);
int rlpx_io_connect(
    rlpx_channel* ch,
    const uecc_public_key* to,
    const char* host,
    uint32_t tcp);
int rlpx_io_connect_enode(rlpx_channel* ch, const char* enode);
int rlpx_io_connect_node(rlpx_channel* ch, const rlpx_node* node);
int rlpx_io_accept(rlpx_channel* ch, const uecc_public_key* from);
int rlpx_io_send_auth(rlpx_channel* ch);
int rlpx_io_send_hello(rlpx_channel* ch);
int rlpx_io_send_disconnect(rlpx_channel* ch, RLPX_DEVP2P_DISCONNECT_REASON);
int rlpx_io_send_ping(rlpx_channel* ch);
int rlpx_io_send_pong(rlpx_channel* ch);
int rlpx_io_recv(rlpx_channel* ch, const uint8_t* d, size_t l);
int rlpx_io_recv_auth(rlpx_channel*, const uint8_t*, size_t l);
int rlpx_io_recv_ack(rlpx_channel* ch, const uint8_t*, size_t l);

static inline int
rlpx_io_is_connected(rlpx_channel* ch)
{
    return async_io_has_sock(&ch->io);
}

static inline int
rlpx_io_is_ready(rlpx_channel* ch)
{
    return ch->ready;
}

static inline int
rlpx_io_is_shutdown(rlpx_channel* ch)
{
    return ch->shutdown;
}

#ifdef __cplusplus
}
#endif
#endif
