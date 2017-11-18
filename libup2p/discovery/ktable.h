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
#include "ukeccak256.h"
#include "usys_timers.h"

/**
 * @brief Initialize hash table api
 */
KHASH_MAP_INIT_INT64(knode_table, knode);

/**
 * @brief typedef unify interface with our hash table wrapper
 */
typedef khint64_t ktable_key;

/**
 * @brief TODO Not sure if this OK.  Our node table has 8 byte lookup key.  We
 * hash pubkey and only use the first 64 bites. Collision just confuses our
 * table but should not likely happen.
 *
 * @param q
 *
 * @return
 */
static inline ktable_key
ktable_pub_to_key(uecc_public_key* q)
{
    uint8_t pub[65], h[32];
    uecc_qtob(q, pub, 65);
    ukeccak256(&pub[1], 64, h, 32);
    return *(int64_t*)h;
}

/**
 * @brief Forward declaration
 */
typedef struct ktable ktable;

/**
 * @brief Callbacks for ktable
 */
typedef int (*ktable_want_ping_fn)(ktable*, knode* n);
typedef int (*ktable_want_find_fn)(ktable*, knode* n, uint8_t* id, uint32_t);

typedef struct ktable_settings
{
    uint32_t size;
    uint32_t refresh;
    uint32_t pong_timeout;
    ktable_want_ping_fn want_ping;
    ktable_want_find_fn want_find;
} ktable_settings;

/**
 * @brief A list of nodes we know about
 */
typedef struct ktable
{
    ktable_settings settings;   /*!< callers config*/
    void* context;              /*!< callers callback context */
    usys_timers_context timers; /*!< hash table of timers */
    usys_timer_key timerid;     /*!< refresh timer id */
    kh_knode_table_t* nodes;    /*!< node hash lookup */
    knode* recents[3];          /*!< last ping */
} ktable;

/**
 * @brief Initialize a ktable context
 *
 * @param table Adress of table
 */
int ktable_init(ktable* table, ktable_settings* settings, void*);

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
 * @brief Call periodically to maintain table
 *
 * @param self
 */
void ktable_poll(ktable* self);

/**
 * @brief Ping from somewhere. Will add node into table if it is not already in
 * table. Will want to ping this node if it is not in the table yet. Will also
 * update node if it is already in the table.
 *
 * @param self api handle
 * @param key key when inserted
 * @param ip
 * @param tcp
 * @param udp
 * @param id
 *
 * @return
 */
int ktable_ping(
    ktable* self,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id);

/**
 * @brief Pong a device in the table
 *
 * @param self api handle
 * @param key key when inserted
 * @param ip
 * @param tcp
 * @param udp
 * @param id
 *
 * @return 0 ok -1 device does not exist or other error
 */
int ktable_pong(
    ktable* self,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id);

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
