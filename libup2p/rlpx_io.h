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
#include "rlpx_frame.h"
#include "rlpx_handshake.h"
#include "rlpx_node.h"

typedef int (*rlpx_io_ready_fn)(void*);
typedef int (*rlpx_io_recv_fn)(void*, const urlp*);

typedef struct
{
    async_io io;                 /*!< io context for network sys calls */
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
    rlpx_io_ready_fn on_ready;   /*!< callback when ready */
    rlpx_io_recv_fn on_recv;     /*!< callback when receive protocol */
} rlpx_io;

// constructors
rlpx_io* rlpx_io_alloc(uecc_ctx* skey, const uint32_t*, async_io_settings*);
void rlpx_io_free(rlpx_io** ch_p);
int rlpx_io_init(
    rlpx_io* io,
    uecc_ctx* s,
    const uint32_t* listen,
    async_io_settings*);
int rlpx_io_init_mock(rlpx_io*, uecc_ctx*, const uint32_t*, async_io_settings*);
void rlpx_io_deinit(rlpx_io* session);

// methods
void rlpx_io_nonce(rlpx_io* ch);
int rlpx_io_poll(rlpx_io** ch, uint32_t count, uint32_t ms);
int rlpx_io_listen(rlpx_io* io);
int rlpx_io_connect(
    rlpx_io* ch,
    const uecc_public_key* to,
    const char* host,
    uint32_t tcp);
int rlpx_io_connect_enode(rlpx_io* ch, const char* enode);
int rlpx_io_connect_node(rlpx_io* ch, const rlpx_node* node);
int rlpx_io_accept(rlpx_io* ch, const uecc_public_key* from);
int rlpx_io_send_auth(rlpx_io* ch);
int rlpx_io_send(async_io* io);
int rlpx_io_recv(rlpx_io* ch, const uint8_t* d, size_t l);
int rlpx_io_recv_auth(rlpx_io*, const uint8_t*, size_t l);
int rlpx_io_recv_ack(rlpx_io* ch, const uint8_t*, size_t l);

static inline int
rlpx_io_is_connected(rlpx_io* ch)
{
    return async_io_has_sock(&ch->io);
}

static inline int
rlpx_io_is_ready(rlpx_io* ch)
{
    return ch->ready;
}

static inline int
rlpx_io_default_on_ready(void* io)
{
    return -1;
}

static inline int
rlpx_io_is_shutdown(rlpx_io* ch)
{
    return ch->shutdown;
}

#ifdef __cplusplus
}
#endif
#endif
