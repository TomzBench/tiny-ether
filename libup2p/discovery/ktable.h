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

//#ifndef KTABLE_H_
//#define KTABLE_H_
//
//#ifdef __cplusplus
// extern "C" {
//#endif

#include "knodes.h"
#include "ukeccak256.h"
#include "utimers.h"

#define KTABLE_N_NODES (20)
#define KTABLE_N_TIMERS (KTABLE_N_NODES + 1)

/**
 * @brief Forward declaration
 */
typedef struct ktable ktable;

/**
 * @brief Callbacks for ktable
 */
typedef int (*ktable_want_ping_fn)(ktable*, knodes* n);
typedef int (*ktable_want_find_fn)(ktable*, knodes* n, uint8_t* id, uint32_t);

typedef struct ktable_settings
{
    uint32_t size;
    uint32_t refresh;
    uint32_t pong_timeout;
    ktable_want_ping_fn want_ping;
    ktable_want_find_fn want_find;
} ktable_settings;

typedef struct
{
    uint8_t h32[32];
} ktable_keys;

/**
 * @brief A list of nodes we know about
 */
typedef struct ktable
{
    ktable_settings settings;         /*!< callers config*/
    void* context;                    /*!< callers callback context */
    int timerid;                      /*!< refresh timer id */
    ktable_keys keys[KTABLE_N_NODES]; /*!< */
    utimers timers[KTABLE_N_TIMERS];  /*!< */
    knodes nodes[KTABLE_N_NODES];     /*!< */
    knodes* recents[3];               /*!< last ping */
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
 * @brief Return a node index from a public key
 *
 * @param self
 * @param q
 *
 * @return
 */
knode_key ktable_pub_to_key(ktable* self, uecc_public_key* q);

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
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp);

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
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp);

/**
 * @brief Make this "node" a most recently heard from node
 *
 * @param table
 * @param node
 */
void ktable_update_recent(ktable* table, knodes* node);

/**
 * @brief Get a node from the table
 *
 * @param self api handle
 * @param key callers hash lookup
 *
 * @return the node or NULL if it does not exist
 */
knodes* ktable_get(ktable* self, uecc_public_key* q);

/**
 * @brief Add a node to our table using rlp data received from find node reply
 *
 * @param table
 * @param rlp
 *
 * @return
 */
knode_key ktable_insert_rlp(ktable* table, uecc_public_key*, const urlp* rlp);

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
knode_key ktable_insert(
    ktable* table,
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    urlp* meta);

/**
 * @brief Remove a node from the table
 *
 * @param self
 * @param key
 */
void ktable_remove(ktable* self, knode_key key);

//#ifdef __cplusplus
//}
//#endif
//#endif
