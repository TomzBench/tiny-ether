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

#ifndef RLPX_DEVP2P_H_
#define RLPX_DEVP2P_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "rlpx_frame.h"
#include "urlp.h"

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

typedef int (*rlpx_protocol_cb)(void* ctx, const urlp* rlp);
typedef struct
{
    rlpx_protocol_cb on_hello;
    rlpx_protocol_cb on_ping;
    rlpx_protocol_cb on_pong;
    rlpx_protocol_cb on_disconnect;
} rlpx_io_devp2p_settings;

typedef struct rlpx_devp2p_protocol
{
    void* ctx;
    int (*recv)(struct rlpx_devp2p_protocol*, const urlp*);
    const rlpx_io_devp2p_settings* settings; /*!< user callbacks */
    char client[RLPX_CLIENT_MAX_LEN];        /*!< Hello packet client*/
    uint32_t listen_port;                    /*!< */
    int64_t ping;                            /*!< ping now() */
    uint32_t latency;                        /*!< now() - ping */
} rlpx_devp2p_protocol;

// Heap constructors
rlpx_devp2p_protocol* rlpx_io_devp2p_alloc(
    const rlpx_io_devp2p_settings* settings,
    void* ctx);
void rlpx_io_devp2p_free(rlpx_devp2p_protocol** self_p);

// Initializers/Deinitializers
void rlpx_io_devp2p_init(
    rlpx_devp2p_protocol* self,
    const rlpx_io_devp2p_settings* settings,
    void* ctx);
void rlpx_io_devp2p_deinit(rlpx_devp2p_protocol* self);

int rlpx_io_devp2p_write(
    rlpx_coder* x,
    RLPX_DEVP2P_PROTOCOL_PACKET_TYPE type,
    urlp* rlp,
    uint8_t* out,
    uint32_t* outlen);
int rlpx_io_devp2p_write_hello(
    rlpx_coder* x,
    uint32_t port,
    const uint8_t* id,
    uint8_t* out,
    uint32_t* l);
int rlpx_io_devp2p_write_disconnect(
    rlpx_coder* x,
    RLPX_DEVP2P_DISCONNECT_REASON reason,
    uint8_t* out,
    uint32_t* l);
int rlpx_io_devp2p_write_ping(rlpx_coder* x, uint8_t* out, uint32_t* l);
int rlpx_io_devp2p_write_pong(rlpx_coder* x, uint8_t* out, uint32_t* l);

static inline int
rlpx_io_devp2p_p2p_version(const urlp* rlp, uint32_t* out)
{
    return urlp_idx_to_u32(rlp, 0, out);
}

static inline int
rlpx_io_devp2p_client_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    rlp = urlp_at(rlp, 1);
    if (rlp) {
        *ptr_p = urlp_ref(rlp, l);
        return 0;
    }
    return -1;
}

static inline int
rlpx_io_devp2p_capabilities(const urlp* rlp, const char* cap, uint32_t v)
{
    const urlp *seek, *caps = urlp_at(rlp, 2);
    uint32_t ver, sz, len = strlen(cap), n = caps ? urlp_siblings(caps) : 0;
    const char* mem;
    for (uint32_t i = 0; i < n; i++) {
        if (!(seek = urlp_at(caps, i))) continue;
        rlp = urlp_at(seek, 0);
        if (rlp) {
            mem = urlp_ref(rlp, &sz);
            if ((sz == len) && (!(memcmp(mem, cap, len)))) {
                urlp_idx_to_u32(seek, 1, &ver);
                return (ver >= v) ? 0 : -1;
            }
        }
    }

    return -1;
}

static inline int
rlpx_io_devp2p_listen_port(const urlp* rlp, uint32_t* port)
{
    return urlp_idx_to_u32(rlp, 3, port);
}

static inline int
rlpx_io_devp2p_node_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    rlp = urlp_at(rlp, 4);
    if (rlp) {
        *ptr_p = urlp_ref(rlp, l);
        return 0;
    }
    return -1;
}

#ifdef __cplusplus
}
#endif
#endif
