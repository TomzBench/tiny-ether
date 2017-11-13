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
#include "unonce.h"

typedef int (*rlpx_io_ready_fn)(void*);
typedef int (*rlpx_io_recv_fn)(void*, const urlp*);
typedef void (*rlpx_io_uninstall_fn)(void**);

typedef struct
{
    void* context;
    rlpx_io_ready_fn ready;
    rlpx_io_recv_fn recv;
    rlpx_io_uninstall_fn uninstall;
} rlpx_io_protocol;

typedef struct rlpx
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
    int error;                   /*!< erro state */
    uint8_t node_id[65];         /*!< node id */
    const uint32_t* listen_port; /*!< our listen port */
    rlpx_io_protocol protocols[RLPX_IO_MAX_PROTOCOL]; /*!< map */
} rlpx_io;

// constructors
rlpx_io* rlpx_io_alloc(uecc_ctx* skey, const uint32_t* listen);
void rlpx_io_free(rlpx_io** ch_p);
void rlpx_io_tcp_init(rlpx_io* io, uecc_ctx* s, const uint32_t* listen);
void rlpx_io_udp_init(rlpx_io* io, uecc_ctx* s, const uint32_t* listen);
void rlpx_io_init(rlpx_io* io, uecc_ctx* s, const uint32_t* listen);
void rlpx_io_deinit(rlpx_io* io);
void rlpx_io_refresh(rlpx_io* rlpx);

// methods
int rlpx_io_poll(rlpx_io** ch, uint32_t count, uint32_t ms);
int rlpx_io_listen(rlpx_io* io);
int rlpx_io_connect(
    rlpx_io* ch,
    const uecc_public_key* to,
    uint32_t ip,
    uint32_t tcp);
int rlpx_io_connect_host(
    rlpx_io* ch,
    const uecc_public_key* to,
    const char* host,
    uint32_t tcp);
int rlpx_io_connect_enode(rlpx_io* ch, const char* enode);
int rlpx_io_connect_node(rlpx_io* ch, const rlpx_node* node);
int rlpx_io_accept(rlpx_io* ch, const uecc_public_key* from);
int rlpx_io_send_auth(rlpx_io* ch);
int rlpx_io_send(async_io* io);
int rlpx_io_send_sync(async_io* io);
int rlpx_io_sendto(async_io* io, uint32_t ip, uint32_t port);
int rlpx_io_sendto_sync(async_io* udp, uint32_t ip, uint32_t port);
int rlpx_io_parse_udp(
    const uint8_t* b,
    uint32_t l,
    uecc_public_key* node_id,
    int* type,
    urlp** rlp);
int rlpx_io_recv_udp(rlpx_io* ch, const uint8_t* b, size_t l);
int rlpx_io_recv(rlpx_io* ch, const uint8_t* d, size_t l);
int rlpx_io_recv_auth(rlpx_io*, const uint8_t*, size_t l);
int rlpx_io_recv_ack(rlpx_io* ch, const uint8_t*, size_t l);

static inline void
rlpx_io_nonce(rlpx_io* io)
{
    unonce(io->nonce.b);
}

static inline int
rlpx_io_error_get(rlpx_io* io)
{
    return io->error;
}

static inline void
rlpx_io_error_set(rlpx_io* io, int error)
{
    io->error = error;
}

static const uecc_public_key*
rlpx_io_spub(rlpx_io* tcp)
{
    return &tcp->skey->Q;
}

static const uecc_public_key*
rlpx_io_spub_remote(rlpx_io* tcp)
{
    return &tcp->node.id;
}

static const uecc_public_key*
rlpx_io_epub(rlpx_io* tcp)
{
    return &tcp->ekey.Q;
}

static const uecc_public_key*
rlpx_io_epub_remote(rlpx_io* tcp)
{
    return tcp->hs ? &tcp->hs->ekey_remote : NULL;
}

static inline uint8_t*
rlpx_io_buffer(rlpx_io* io)
{
    return async_io_buffer((async_io*)io);
}

static inline uint32_t*
rlpx_io_len_ptr(rlpx_io* io)
{
    return async_io_buffer_length_pointer((async_io*)io);
}

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
rlpx_io_default_on_recv(void* io, const urlp* rlp)
{
    return -1;
}

static inline int
rlpx_io_is_shutdown(rlpx_io* ch)
{
    return ch->shutdown;
}

static inline void
rlpx_io_close(rlpx_io* io)
{
    async_io_close(&io->io);
    io->shutdown = 1;
}

#ifdef __cplusplus
}
#endif
#endif
