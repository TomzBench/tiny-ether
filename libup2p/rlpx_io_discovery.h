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

#define RLPX_IO_DISCOVERY_TABLE_SIZE 100

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
 * @brief Do we like this peer?
 */
typedef enum {
    RLPX_STATE_FREE = 0,
    RLPX_STATE_PENDING = 1,
    RLPX_STATE_CONNECTING = 2,
    RLPX_STATE_TRUE = 3,
    RLPX_STATE_FALSE = 4
} RLPX_DISCOVERY_STATE;

/**
 * @brief IPv4|IPv6 Endpoint data
 */
typedef struct
{
    uint32_t ip;  /*!< ipv4 only */
    uint32_t tcp; /*!< devp2p port */
    uint32_t udp; /*!< p2p port */
} rlpx_io_discovery_endpoint;

/**
 * @brief Type of endpoint with additional node id (static key) and usefulness
 * Can be down casted to rlpx_io_discovery_endpoint
 */
typedef struct
{
    rlpx_io_discovery_endpoint ep; /*!< remote endpoint routing*/
    uecc_public_key nodeid;        /*!< pubkey */
    RLPX_DISCOVERY_STATE state;    /*!< usefulness */
} rlpx_io_discovery_endpoint_node;

/**
 * @brief A list of nodes we know about
 */
typedef struct
{
    rlpx_io_discovery_endpoint_node nodes[RLPX_IO_DISCOVERY_TABLE_SIZE];
    rlpx_io_discovery_endpoint_node* recents[3]; /*!< last ping */
} rlpx_io_discovery_table;

/**
 * @brief base class
 */
typedef struct
{
    rlpx_io* base;
    void* ctx;
    rlpx_io_discovery_table table;
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
 * @brief Initialize a rlpx_io_discovery_table context
 *
 * @param table Adress of table
 */
void rlpx_io_discovery_table_init(rlpx_io_discovery_table* table);

/**
 * @brief Initialize an IPv4 endpoint context
 *
 * @param ep
 * @param ip
 * @param udp
 * @param tcp
 */
void rlpx_io_discovery_endpoint_v4_init(
    rlpx_io_discovery_endpoint* ep,
    uint32_t ip,
    uint32_t udp,
    uint32_t tcp);

/**
 * @brief Initialize an IPv6 endpoint context
 *
 * @param ep
 * @param ipv6
 * @param udp
 * @param tcp
 */
void rlpx_io_discovery_endpoint_v6_init(
    rlpx_io_discovery_endpoint* ep,
    uint8_t* ipv6,
    uint32_t udp,
    uint32_t tcp);

/**
 * @brief Find a node in our table using public key in point format
 *
 * @param table
 * @param target
 * @param node
 *
 * @return
 */
int rlpx_io_discovery_table_find_node(
    rlpx_io_discovery_table* table,
    uecc_public_key* target,
    rlpx_io_discovery_endpoint_node* node);

/**
 * @brief Make this "node" a most recently heard from node
 *
 * @param table
 * @param node
 */
void rlpx_io_discovery_table_update_recent(
    rlpx_io_discovery_table* table,
    rlpx_io_discovery_endpoint_node* node);

/**
 * @brief Add a node to our table using rlp data received from find node reply
 *
 * @param table
 * @param rlp
 *
 * @return
 */
int rlpx_io_discovery_table_node_add_rlp(
    rlpx_io_discovery_table* table,
    const urlp* rlp);

/**
 * @brief Add a node to out table using raw data
 *
 * @param table
 * @param ip
 * @param iplen
 * @param tcp
 * @param udp
 * @param id
 * @param meta
 *
 * @return
 */
int rlpx_io_discovery_table_node_add(
    rlpx_io_discovery_table* table,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta);

/**
 * @brief Seek a node in the table. (pass null to receive empty node)
 *
 * @param table
 *
 * @return
 */
rlpx_io_discovery_endpoint_node* rlpx_io_discovery_table_node_get_id(
    rlpx_io_discovery_table* table,
    const uecc_public_key* id);

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
 * @brief Convert RLP endpoint data into end point struct
 * (rename rlpx_io_discovery_rlp_to_ep)?
 *
 * @param urlp
 * @param ep
 *
 * @return
 */
int rlpx_io_discovery_rlp_to_endpoint(
    const urlp*,
    rlpx_io_discovery_endpoint* ep);

/**
 * @brief Return RLP from an endpoint struct
 * (rename rlpx_io_discovery_ep_to_rlp)?
 *
 * @param ep
 *
 * @return
 */
urlp* rlpx_io_discovery_endpoint_to_rlp(const rlpx_io_discovery_endpoint* ep);

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
    rlpx_io_discovery_endpoint* from,
    rlpx_io_discovery_endpoint* to,
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
    rlpx_io_discovery_endpoint* to,
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
    const rlpx_io_discovery_endpoint* ep_src,
    const rlpx_io_discovery_endpoint* ep_dst,
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
    const rlpx_io_discovery_endpoint* ep_to,
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
    rlpx_io_discovery_table* t,
    uint32_t timestamp,
    uint8_t* d,
    uint32_t* l);

int rlpx_io_discovery_send_ping(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const rlpx_io_discovery_endpoint* ep_src,
    const rlpx_io_discovery_endpoint* ep_dst,
    uint32_t timestamp);

int rlpx_io_discovery_send_pong(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const rlpx_io_discovery_endpoint* ep_to,
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
    rlpx_io_discovery_table* table,
    uint32_t timestamp);
#ifdef __cplusplus
}
#endif
#endif
