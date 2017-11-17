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

#include "khash.h"
#include "knode.h"
#include "usys_timers.h"

#define KTABLE_SIZE 40

/**
 * @brief Initialize hash table api
 */
KHASH_MAP_INIT_INT(knode_table, knode);

/**
 * @brief typedef unify interface with our hash table wrapper
 */
typedef khint_t ktable_key;

/**
 * @brief A list of nodes we know about
 */
typedef struct
{
    uint32_t max;
    kh_knode_table_t* nodes;
    knode* recents[3]; /*!< last ping */
} ktable;

/**
 * @brief Initialize a ktable context
 *
 * @param table Adress of table
 */
int ktable_init(ktable* table);

/**
 * @brief Free table after no longer need
 *
 * @param table
 */
void ktable_deinit(ktable* table);

/**
 * @brief Return the number of nodes in the table
 *
 * @param self
 *
 * @return
 */
uint32_t ktable_size(ktable* self);

/**
 * @brief Make this "node" a most recently heard from node
 *
 * @param table
 * @param node
 */
void ktable_update_recent(ktable* table, knode* node);

/**
 * @brief Get a node from the table
 *
 * @param self api handle
 * @param key callers hash lookup
 *
 * @return the node or NULL if it does not exist
 */
knode* ktable_get(ktable* self, ktable_key key);

/**
 * @brief Add a node to our table using rlp data received from find node reply
 *
 * @param table
 * @param rlp
 *
 * @return
 */
ktable_key ktable_insert_rlp(ktable* table, ktable_key key, const urlp* rlp);

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
ktable_key ktable_insert(
    ktable* table,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta);

/**
 * @brief Remove a node from the table
 *
 * @param self
 * @param key
 */
void ktable_remove(ktable* self, ktable_key key);

#ifdef __cplusplus
}
#endif
#endif
