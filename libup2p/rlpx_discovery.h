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
#include "uecc.h"
#include "urlp.h"
#include "usys_io.h"

typedef enum {
    RLPX_DISCOVERY_PING = 1,
    RLPX_DISCOVERY_PONG = 2,
    RLPX_DISCOVERY_FIND = 3,
    RLPX_DISCOVERY_NEIGHBOURS = 4
} RLPX_DISCOVERY;

typedef enum {
    RLPX_USEFUL_FREE = 0,
    RLPX_USEFUL_PENDING = 1,
    RLPX_USEFUL_TRUE = 2,
    RLPX_USEFUL_FALSE = 3
} RLPX_DISCOVERY_USEFUL;

typedef struct
{
    uint8_t ip[16]; /*!< BE encoding ipv4 or ipv6 data */
    uint32_t iplen; /*!< ip4|6 */
    uint32_t tcp;   /*!< devp2p port */
    uint32_t udp;   /*!< p2p port */
} rlpx_discovery_endpoint;

typedef struct
{
    uecc_public_key nodeid;       /*!< pubkey */
    rlpx_discovery_endpoint ep;   /*!< remote endpoint routing*/
    RLPX_DISCOVERY_USEFUL useful; /*!< usefulness */
} rlpx_discovery_node;

typedef struct
{
    rlpx_discovery_node nodes[10];   /*!< potential peers */
    rlpx_discovery_node* recents[3]; /*!< ptrs to most recent pings */
} rlpx_discovery_table;

/**
 * @brief
 *
 * @param table
 */
void rlpx_discovery_table_init(rlpx_discovery_table* table);

int rlpx_discovery_table_find_node(rlpx_discovery_table* table,
                                   uecc_public_key* target,
                                   rlpx_discovery_node* node);

void rlpx_discovery_table_update_recent(rlpx_discovery_table* table,
                                        rlpx_discovery_node* node);
/**
 * @brief
 *
 * @param table
 * @param rlp
 *
 * @return
 */
int rlpx_discovery_table_add_node_rlp(rlpx_discovery_table* table,
                                      const urlp* rlp);

/**
 * @brief
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
int rlpx_discovery_table_add_node(rlpx_discovery_table* table,
                                  uint8_t* ip,
                                  uint32_t iplen,
                                  uint32_t tcp,
                                  uint32_t udp,
                                  uecc_public_key* id,
                                  urlp* meta);

int rlpx_discovery_recv(rlpx_discovery_table* t, const uint8_t* b, uint32_t l);
int rlpx_discovery_parse(const uint8_t* b,
                         uint32_t l,
                         uecc_public_key* node_id,
                         int* type,
                         urlp** rlp);

int rlpx_discovery_parse_endpoint(const urlp*, rlpx_discovery_endpoint* ep);

int rlpx_discovery_parse_ping(const urlp**,
                              uint8_t* version32,
                              rlpx_discovery_endpoint* from,
                              rlpx_discovery_endpoint* to,
                              uint32_t* timestamp);
int rlpx_discovery_print_ping(uint32_t ver,
                              const rlpx_discovery_endpoint* ep_src,
                              const rlpx_discovery_endpoint* ep_dst,
                              uint32_t timestamp,
                              uint8_t* dst,
                              uint32_t* l);
int rlpx_discovery_parse_pong(const urlp** rlp,
                              rlpx_discovery_endpoint* to,
                              uint8_t* echo32,
                              uint32_t* timestamp);
int rlpx_discovery_print_pong(uint32_t timestamp,
                              h256* echo,
                              const rlpx_discovery_endpoint* ep_to,
                              uint8_t* d,
                              uint32_t* l);
int rlpx_discovery_parse_find(const urlp** rlp,
                              uecc_public_key* q,
                              uint32_t* ts);
int rlpx_discovery_print_find(uint8_t* nodeid,
                              uint32_t timestamp,
                              uint8_t* b,
                              uint32_t* l);
int rlpx_discovery_parse_neighbours(rlpx_discovery_table* t, const urlp** rlp);
//  rlpx_discovery_print_neighbours( ....TODO

#ifdef __cplusplus
}
#endif
#endif
