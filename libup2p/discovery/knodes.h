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

//#ifndef KNODE_H_
//#define KNODE_H_
//
//#ifdef __cplusplus
// extern "C" {
//#endif

#include "rlpx_config.h"
#include "uecc.h"
#include "urlp.h"
#include "utimers.h"

/**
 * @brief Do we like this peer?
 */

#define KNODES_EMPTY 0x01
#define KNODES_PENDING 0x02
#define KNODES_CONNECTING 0x04

#define KNODES_IS_EMPTY(n) (n.flags & KNODES_EMPTY)

typedef int knode_key;

/**
 * @brief Type of endpoint with additional node id (static key) and usefulness
 */
typedef struct knodes
{
    uint32_t ip, tcp, udp;  /*!< endpoing data*/
    uint8_t flags;          /*!< */
    knode_key key;          /*!< hash lookup*/
    uecc_public_key nodeid; /*!< pubkey */
} knodes;

static inline void
knodes_init(knodes* nodes, int count)
{
    memset(nodes, 0, sizeof(knodes) * count);
    for (int i = 0; i < count; i++) nodes[i].flags |= KNODES_EMPTY;
}

static inline void
knodes_deinit(knodes* nodes, int count)
{
    memset(nodes, 0, sizeof(knodes) * count);
}

static inline uint32_t
knodes_size(knodes* nodes, int count)
{
    uint32_t total = 0;
    for (int i = 0; i < count; i++) {
        if (!(KNODES_IS_EMPTY(nodes[i]))) total++;
    }
    return total;
}

static inline knodes*
knodes_get(knodes* nodes, knode_key idx)
{
    return KNODES_IS_EMPTY(nodes[idx]) ? NULL : &nodes[idx];
}

static inline int
knodes_insert(
    knodes* nodes,
    knode_key idx,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* q)
{
    nodes[idx].flags = 0;
    nodes[idx].ip = ip;
    nodes[idx].tcp = tcp;
    nodes[idx].udp = udp;
    nodes[idx].key = idx;
    if (q) nodes[idx].nodeid = *q;
    return 0;
}

static inline knode_key
knodes_insert_free(
    knodes* nodes,
    int count,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* q)
{
    knode_key key = 0;
    for (key = 0; key < count; key++) {
        if (KNODES_IS_EMPTY(nodes[key])) {
            knodes_insert(nodes, key, ip, tcp, udp, q);
            return key;
        }
    }
    return -1;
}

static inline int
knodes_remove(knodes* n, knode_key idx)
{
    n[idx].flags |= KNODES_EMPTY;
    return 0;
}

/**
 * @brief Convert RLP endpoint data into end point struct
 * (rename rlpx_io_discovery_rlp_to_ep)?
 *
 * @param urlp
 * @param ep
 *
 * @return
 */
int knodes_rlp_to_node(const urlp*, knodes* ep);

/**
 * @brief Return RLP from an endpoint struct
 * (rename rlpx_io_discovery_ep_to_rlp)?
 *
 * @param ep
 *
 * @return
 */
urlp* knodes_node_to_rlp(const knodes* ep);

//#ifdef __cplusplus
//}
//#endif
//#endif
