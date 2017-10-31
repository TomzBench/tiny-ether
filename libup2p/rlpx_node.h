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
 * @file rlpx_node.h
 *
 * @brief Endpoint context for storing methods and metadata for a remote
 * endpoint.
 */
#ifndef RLPX_NODE_H_
#define RLPX_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uecc.h"

typedef struct
{
    uecc_public_key id;
    char ip_v4[16];
    uint32_t port_tcp;
    uint32_t port_udp;
} rlpx_node;

int rlpx_node_init(
    rlpx_node* self,
    const uecc_public_key* id,
    const char* host,
    uint32_t tcp,
    uint32_t udp);

/**
 * @brief Create a node from an enode string.
 *
 * Format: enode://[128]@[ip]:[tcp](.udp)
 *
 *
 * @param self
 * @param enode
 *
 * @return
 */
int rlpx_node_init_enode(rlpx_node* self, const char* enode);

void rlpx_node_deinit(rlpx_node* self);

int rlpx_node_hex_to_bin(const char*, uint32_t, uint8_t*, uint32_t*);
int rlpx_node_bin_to_hex(const uint8_t*, uint32_t, char*, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
