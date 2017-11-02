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
    RLPX_USEFUL_FREE = 0,
    RLPX_USEFUL_PENDING = 1,
    RLPX_USEFUL_TRUE = 2,
    RLPX_USEFUL_FALSE = 3
} RLPX_DISCOVERY_USEFUL;

/**
 * @brief IPv4|IPv6 Endpoint data
 */
typedef struct
{
    uint8_t ip[16]; /*!< BE encoding ipv4 or ipv6 data */
    uint32_t iplen; /*!< ip4|6 */
    uint32_t tcp;   /*!< devp2p port */
    uint32_t udp;   /*!< p2p port */
} rlpx_io_discovery_endpoint;

/**
 * @brief Type of endpoint with additional node id (static key) and usefulness
 * Can be down casted to rlpx_io_discovery_endpoint
 */
typedef struct
{
    rlpx_io_discovery_endpoint ep; /*!< remote endpoint routing*/
    uecc_public_key nodeid;        /*!< pubkey */
    RLPX_DISCOVERY_USEFUL useful;  /*!< usefulness */
} rlpx_io_discovery_endpoint_node;

/**
 * @brief A list of nodes we know about
 */
typedef struct
{
    rlpx_io_discovery_endpoint_node nodes[10];   /*!< potential peers */
    rlpx_io_discovery_endpoint_node* recents[3]; /*!< last ping */
} rlpx_io_discovery_table;

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
int rlpx_io_discovery_table_add_node_rlp(
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
int rlpx_io_discovery_table_add_node(
    rlpx_io_discovery_table* table,
    uint8_t* ip,
    uint32_t iplen,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta);

/**
 * @brief The protocol recv function.
 *
 * @param t
 * @param b
 * @param l
 *
 * @return
 */
int rlpx_io_discovery_recv(
    rlpx_io_discovery_table* t,
    const uint8_t* b,
    uint32_t l);
int rlpx_io_discovery_parse(
    const uint8_t* b,
    uint32_t l,
    uecc_public_key* node_id,
    int* type,
    urlp** rlp);
int rlpx_io_discovery_write(
    uecc_ctx* key,
    RLPX_DISCOVERY type,
    const urlp* rlp,
    uint8_t* b,
    uint32_t* l);
int rlpx_io_discovery_parse_endpoint(
    const urlp*,
    rlpx_io_discovery_endpoint* ep);
urlp* rlpx_io_discovery_rlp_endpoint(const rlpx_io_discovery_endpoint* ep);
int rlpx_io_discovery_parse_ping(
    const urlp**,
    uint8_t* version32,
    rlpx_io_discovery_endpoint* from,
    rlpx_io_discovery_endpoint* to,
    uint32_t* timestamp);
int rlpx_io_discovery_parse_pong(
    const urlp** rlp,
    rlpx_io_discovery_endpoint* to,
    uint8_t* echo32,
    uint32_t* timestamp);
int rlpx_io_discovery_parse_find(
    const urlp** rlp,
    uecc_public_key* q,
    uint32_t* ts);
int rlpx_io_discovery_parse_neighbours(
    rlpx_io_discovery_table* t,
    const urlp** rlp);
int rlpx_io_discovery_write_ping(
    uecc_ctx* skey,
    uint32_t ver,
    const rlpx_io_discovery_endpoint* ep_src,
    const rlpx_io_discovery_endpoint* ep_dst,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l);
int rlpx_io_discovery_write_pong(
    uecc_ctx* skey,
    const rlpx_io_discovery_endpoint* ep_to,
    h256* echo,
    uint32_t timestamp,
    uint8_t* d,
    uint32_t* l);
int rlpx_io_discovery_write_find(
    uecc_ctx* skey,
    uecc_public_key* nodeid,
    uint32_t timestamp,
    uint8_t* b,
    uint32_t* l);
int rlpx_io_discovery_write_neighbours(
    uecc_ctx* skey,
    rlpx_io_discovery_table* t,
    uint32_t timestamp,
    uint8_t* d,
    uint32_t* l);

#ifdef __cplusplus
}
#endif
#endif