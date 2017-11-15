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

#ifndef KTABLE_H_
#define KTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "knode.h"

#define KTABLE_SIZE 40

/**
 * @brief A list of nodes we know about
 */
typedef struct
{
    knode nodes[KTABLE_SIZE];
    knode* recents[3]; /*!< last ping */
} ktable;

/**
 * @brief Initialize a ktable context
 *
 * @param table Adress of table
 */
void ktable_init(ktable* table);

/**
 * @brief Find a node in our table using public key in point format
 *
 * @param table
 * @param target
 * @param node
 *
 * @return
 */
int ktable_find_node(ktable* table, uecc_public_key* target, knode* node);

/**
 * @brief Make this "node" a most recently heard from node
 *
 * @param table
 * @param node
 */
void ktable_update_recent(ktable* table, knode* node);

/**
 * @brief Add a node to our table using rlp data received from find node reply
 *
 * @param table
 * @param rlp
 *
 * @return
 */
int ktable_node_add_rlp(ktable* table, const urlp* rlp);

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
int ktable_node_add(
    ktable* table,
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
knode* ktable_node_get_id(ktable* table, const uecc_public_key* id);

#ifdef __cplusplus
}
#endif
#endif
