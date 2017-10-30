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

typedef struct
{
    uint8_t ip[16]; /*!< BE encoding ipv4 or ipv3 data */
    uint32_t tcp;   /*!< devp2p port */
    uint32_t udp;   /*!< p2p port */
} rlpx_discovery_endpoint;

typedef struct
{
    uecc_public_key nodeid;     /*!< pubkey */
    rlpx_discovery_endpoint ep; /*!< remote endpoint routing*/
    int useful;                 /*!< -1=discard, 0=free/unknown, 1=likey */
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
                                  uint8_t ip,
                                  uint32_t iplen,
                                  uint32_t tcp,
                                  uint32_t udp,
                                  uecc_public_key* id,
                                  urlp* meta);

int rlpx_discovery_recv(usys_sockaddr* addr, const uint8_t* b, uint32_t l);
int rlpx_discovery_parse(usys_sockaddr* addr,
                         const uint8_t* b,
                         uint32_t l,
                         uecc_public_key* node_id,
                         int* type,
                         urlp** rlp);

int rlpx_discovery_parse_ping(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_ping(uint32_t ver,
                              uint32_t timestamp,
                              const rlpx_discovery_endpoint* ep_src,
                              const rlpx_discovery_endpoint* ep_dst,
                              uint8_t* dst,
                              uint32_t* l);
int rlpx_discovery_parse_pong(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_pong(uint32_t timestamp,
                              h256* echo,
                              const rlpx_discovery_endpoint* ep_to,
                              uint8_t* d,
                              uint32_t* l);
int rlpx_discovery_parse_find(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_find(uint8_t* nodeid,
                              uint32_t timestamp,
                              uint8_t* b,
                              uint32_t* l);
int rlpx_discovery_parse_neighbours(usys_sockaddr* addr, const urlp** rlp);
//  rlpx_discovery_print_neighbours( ....TODO

#ifdef __cplusplus
}
#endif
#endif
