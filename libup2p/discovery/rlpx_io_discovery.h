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

#ifndef RLPX_DISCOVERY_H_
#define RLPX_DISCOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "rlpx_io.h"

#include "knode.h"
#include "ktable.h"

/**
 * @brief Discovery protocol packet types
 */
typedef enum {
    RLPX_DISCOVERY_PING = 1,
    RLPX_DISCOVERY_PONG = 2,
    RLPX_DISCOVERY_FIND = 3,
    RLPX_DISCOVERY_NEIGHBOURS = 4
} RLPX_DISCOVERY;

/**
 * @brief base class
 */
typedef struct
{
    rlpx_io* base;
    void* ctx;
    ktable table;
} rlpx_io_discovery;

/**
 * @brief
 *
 * @param self
 * @param base
 */
void rlpx_io_discovery_init(rlpx_io_discovery* self, rlpx_io* base);

/**
 * @brief
 *
 * @param base
 *
 * @return
 */
int rlpx_io_discovery_install(rlpx_io* base);

/**
 * @brief
 *
 * @param ptr_p
 */
void rlpx_io_discovery_uninstall(void** ptr_p);

/**
 * @brief
 *
 * @param rlpx
 *
 * @return
 */
rlpx_io_discovery* rlpx_io_discovery_get_context(rlpx_io* rlpx);

/**
 * @brief Connect to some node in our table if we have some peers in table
 *
 * @param self
 * @param ch
 *
 * @return
 */
int rlpx_io_discovery_connect(rlpx_io_discovery* self, rlpx_io* ch);

/**
 * @brief Dummy function to populate a callback not used during udp for now.
 *
 * @return
 */
int rlpx_io_discovery_ready(void*);

/**
 * @brief Callback when receive
 *
 * @param rlp
 *
 * @return
 */
int rlpx_io_discovery_recv(void*, const urlp* rlp);

/**
 * @brief Parse a signed rlp ping packet
 *
 * @param
 * @param version32
 * @param from
 * @param to
 * @param timestamp
 *
 * @return
 */
int rlpx_io_discovery_recv_ping(
    const urlp**,
    uint8_t* version32,
    knode* from,
    knode* to,
    uint32_t* timestamp);

/**
 * @brief Parse a signed rlp pong packet
 *
 * @param rlp
 * @param to
 * @param echo32
 * @param timestamp
 *
 * @return
 */
int rlpx_io_discovery_recv_pong(
    const urlp** rlp,
    knode* to,
    uint8_t* echo32,
    uint32_t* timestamp);

/**
 * @brief Parse a signed rlp find packet
 *
 * @param rlp
 * @param q
 * @param ts
 *
 * @return
 */
int
rlpx_io_discovery_recv_find(const urlp** rlp, uecc_public_key* q, uint32_t* ts);

/**
 * @brief Parse a signed rlp neighbours packet
 *
 * @param t
 * @param rlp
 *
 * @return
 */
int rlpx_io_discovery_recv_neighbours(
    const urlp** rlp, //
    urlp_walk_fn fn,
    void* ctx);

/**
 * @brief sign a discovery packet provided RLP and a packet type
 *
 * @param key
 * @param type
 * @param rlp
 * @param b
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_write(
    uecc_ctx* key,
    RLPX_DISCOVERY type,
    const urlp* rlp,
    uint8_t* b,
    uint32_t* l);

/**
 * @brief Populate output buffer with signed ping packet
 *
 * @param skey
 * @param ver
 * @param ep_src
 * @param ep_dst
 * @param timestamp
 * @param dst
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_write_ping(
    uecc_ctx* skey,
    uint32_t ver,
    const knode* ep_src,
    const knode* ep_dst,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l);

/**
 * @brief Populate output buffer with signed pong packet
 *
 * @param skey
 * @param ep_to
 * @param echo
 * @param timestamp
 * @param d
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_write_pong(
    uecc_ctx* skey,
    const knode* ep_to,
    h256* echo,
    uint32_t timestamp,
    uint8_t* d,
    uint32_t* l);

/**
 * @brief Populate output buffer with signed find packet
 *
 * @param skey
 * @param nodeid
 * @param timestamp
 * @param b
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_write_find(
    uecc_ctx* skey,
    uecc_public_key* nodeid,
    uint32_t timestamp,
    uint8_t* b,
    uint32_t* l);

/**
 * @brief Populate output with signed neighbours packet.
 *
 * (Always sends empty neighbours for now.)
 *
 * @param skey
 * @param t
 * @param timestamp
 * @param d
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_write_neighbours(
    uecc_ctx* skey,
    ktable* t,
    uint32_t timestamp,
    uint8_t* d,
    uint32_t* l);

int rlpx_io_discovery_send_ping(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const knode* ep_src,
    const knode* ep_dst,
    uint32_t timestamp);

int rlpx_io_discovery_send_pong(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const knode* ep_to,
    h256* echo,
    uint32_t timestamp);

int rlpx_io_discovery_send_find(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    uecc_public_key* nodeid,
    uint32_t timestamp);

int rlpx_io_discovery_send_neighbours(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    ktable* table,
    uint32_t timestamp);
#ifdef __cplusplus
}
#endif
#endif
