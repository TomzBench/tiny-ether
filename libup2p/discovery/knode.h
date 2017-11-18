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

#ifndef KNODE_H_
#define KNODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uecc.h"
#include "urlp.h"
#include "usys_timers.h"

/**
 * @brief Do we like this peer?
 */
typedef enum {
    KNODE_STATE_FREE = 0,
    KNODE_STATE_PENDING = 1,
    KNODE_STATE_CONNECTING = 2,
    KNODE_STATE_TRUE = 3,
    KNODE_STATE_FALSE = 4
} KNODE_STATE;

/**
 * @brief Type of endpoint with additional node id (static key) and usefulness
 */
typedef struct
{
    uint32_t ip, tcp, udp;  /*!< endpoing data*/
    khint64_t key;          /*!< hash lookup*/
    usys_timer_key timerid; /*!< random bits */
    uecc_public_key nodeid; /*!< pubkey */
    KNODE_STATE state;      /*!< usefulness */
} knode;

/**
 * @brief Initialize an IPv4 endpoint context
 *
 * @param ep
 * @param ip
 * @param udp
 * @param tcp
 */
void knode_v4_init(
    knode* n,
    uecc_public_key* id,
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
void knode_v6_init(
    knode* ep,
    uecc_public_key* id,
    uint8_t* ipv6,
    uint32_t udp,
    uint32_t tcp);

/**
 * @brief Convert RLP endpoint data into end point struct
 * (rename rlpx_io_discovery_rlp_to_ep)?
 *
 * @param urlp
 * @param ep
 *
 * @return
 */
int knode_rlp_to_node(const urlp*, knode* ep);

/**
 * @brief Return RLP from an endpoint struct
 * (rename rlpx_io_discovery_ep_to_rlp)?
 *
 * @param ep
 *
 * @return
 */
urlp* knode_node_to_rlp(const knode* ep);

#ifdef __cplusplus
}
#endif
#endif
