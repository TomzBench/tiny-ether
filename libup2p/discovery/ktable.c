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

#include "ktable.h"
#include "urand.h"

int ktable_timer_want_pong(utimers* key, void* ctx, uint32_t tick);
int ktable_timer_refresh(utimers* key, void* ctx, uint32_t tick);
void ktable_neighbours_walk(const urlp* rlp, int idx, void* ctx);

int
ktable_init(ktable* table, ktable_settings* settings, void* ctx)
{
    memset(table, 0, sizeof(ktable));
    knodes_init(table->nodes, KTABLE_N_NODES);
    table->settings = *settings;
    table->context = ctx;
    table->timerid = KTABLE_N_TIMERS - 1;
    utimers_init(table->timers, KTABLE_N_TIMERS);
    utimers_insert(table->timers, table->timerid, ktable_timer_refresh, table);
    utimers_start(table->timers, table->timerid, table->settings.refresh);
    return 0;
}

void
ktable_deinit(ktable* table)
{
    knodes_deinit(table->nodes, KTABLE_N_NODES);
    utimers_deinit(table->timers, KTABLE_N_TIMERS);
    memset(table, 0, sizeof(ktable));
}

knode_key
ktable_pub_to_key(ktable* self, uecc_public_key* q)
{
    uint8_t h32[32], pub65[65];
    int i, c;
    uecc_qtob(q, pub65, sizeof(pub65));
    ukeccak256(&pub65[1], 64, h32, 32);
    for (i = 0; i < KTABLE_N_NODES; i++) {
        for (c = 0; c < 32; c++) {
            if (!(self->keys[i].h32[c] == h32[c])) break;
        }
        if (c == 32) break; // all equal
    }
    return i == KTABLE_N_NODES ? -1 : i;
}

void
ktable_poll(ktable* self)
{
    utimers_poll(self->timers, KTABLE_N_TIMERS);
}

int
ktable_ping(ktable* self, uecc_public_key* q)
{
    knodes* n = ktable_get(self, q);
    if (n) {
        utimers_insert(self->timers, n->key, ktable_timer_want_pong, self);
        utimers_start(self->timers, n->key, self->settings.pong_timeout);
        return 0;
    } else {
        return -1;
    }
}

int
ktable_on_ping(
    ktable* self,
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp)
{
    knodes* node = ktable_get(self, q);
    if (!node) {
        // table insert auto pings (we'll see if we need to change that)
        ktable_insert(self, q, ip, tcp, udp, NULL);
        return 0;
    } else {
        if (udp) node->udp = udp;
        if (tcp) node->tcp = tcp;
        return 0;
    }
}

int
ktable_on_pong(
    ktable* self,
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp)
{
    knodes* node = ktable_get(self, q);
    if (node) {
        utimers_cancel(self->timers, node->key);
        if (ip) node->ip = ip;
        if (tcp) node->tcp = tcp;
        if (udp) node->udp = udp;
        if (q) node->nodeid = *q;
        return 0;
    } else {
        // unsolicited pong
        return -1;
    }
}

int
ktable_on_find(ktable* self)
{
    ((void)self);
    return -1;
}

int
ktable_on_neighbours(ktable* self, const urlp** rlp)
{
    const urlp *n = urlp_at(*rlp, 0),              // get list of neighbours
        *ts = urlp_at(*rlp, 1);                    // get timestamp
    ((void)ts);                                    // TODO
    urlp_foreach(n, self, ktable_neighbours_walk); // loop and add to table
    return 0;
}

void
ktable_neighbours_walk(const urlp* rlp, int idx, void* ctx)
{
    // rlp.list(ipv(4|6),udp,tcp,nodeid)
    ((void)idx);
    int err;
    ktable* self = (ktable*)ctx;
    knodes node;
    uint32_t n = urlp_children(rlp), udp, tcp, publen = 64, ip, iplen = 16;
    uint8_t pub[65] = { 0x04 };
    uecc_public_key q;
    if (n < 4) return; /*!< invalid rlp */

    // short circuit bail. Arrive inside no errors
    if (((iplen = urlp_size(urlp_at(rlp, 0))) == 4) &&
        (!(err = urlp_idx_to_u32(rlp, 0, &ip))) &&
        (!(err = urlp_idx_to_u32(rlp, 1, &udp))) &&
        (!(err = urlp_idx_to_u32(rlp, 2, &tcp))) &&
        (!(err = urlp_idx_to_mem(rlp, 3, &pub[1], &publen))) &&
        (!(err = uecc_btoq(pub, publen + 1, &q)))) {
        // TODO - ipv4 only
        // Note - reading the rlp as a uint32 converts to host byte order.  To
        // preserve network byte order than read rlp as mem.  usys networking io
        // takes host byte order so we read rlp into host byte order.
        node.ip = ip;
        node.tcp = tcp;
        node.udp = udp;
        node.nodeid = q;
        node.flags = node.key = 0;
        self->settings.want_ping(self, &node);
    }
}

void
ktable_update_recent(ktable* table, knodes* node)
{
    table->recents[2] = table->recents[1];
    table->recents[1] = table->recents[0];
    table->recents[0] = node;
}

knodes*
ktable_get(ktable* self, uecc_public_key* q)
{
    knode_key key = ktable_pub_to_key(self, q);
    return key >= 0 ? knodes_get(self->nodes, key) : NULL;
}

knode_key
ktable_insert_rlp(ktable* table, uecc_public_key* key, const urlp* rlp)
{
    int err = 0;
    uint32_t n = urlp_children(rlp), udp, tcp, ip, publen = 64, iplen = 16;
    uint8_t pub[65] = { 0x04 };
    uecc_public_key q;
    if (n < 4) return -1; /*!< invalid rlp */

    // short circuit bail. Arrive inside no errors
    if (((iplen = urlp_size(urlp_at(rlp, 0)) == 4)) &&
        (!(err = urlp_idx_to_u32(rlp, 0, &ip))) &&
        (!(err = urlp_idx_to_u32(rlp, 1, &udp))) &&
        (!(err = urlp_idx_to_u32(rlp, 2, &tcp))) &&
        (!(err = urlp_idx_to_mem(rlp, 3, &pub[1], &publen))) &&
        (!(err = uecc_btoq(pub, publen + 1, &q)))) {
        return ktable_insert(table, key, ip, udp, tcp, NULL);
    }
    return 0;
}

knode_key
ktable_insert(
    ktable* self,
    uecc_public_key* q,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    urlp* meta)
{
    ((void)meta);
    knodes* node = ktable_get(self, q);
    knode_key key;
    uint8_t pub[65];
    if (node) {
        node->ip = ip;
        node->tcp = tcp;
        node->udp = udp;
        node->nodeid = *q;
        return 0;
    } else {
        key = knodes_insert_free(self->nodes, KTABLE_N_NODES, ip, tcp, udp, q);
        if (key >= 0) {
            uecc_qtob(q, pub, 65);
            ukeccak256(&pub[1], 64, self->keys[key].h32, 32);
            return key;
        } else {
            return -1; // no room in table ping nodes cache insert
        }
    }
}

void
ktable_remove(ktable* self, uecc_public_key* q)
{
    knode_key key = ktable_pub_to_key(self, q);
    if (key >= 0) ktable_remove_key(self, key);
}

void
ktable_remove_key(ktable* self, knode_key key)
{
    knodes_remove(self->nodes, key);
}

int
ktable_timer_want_pong(utimers* t, void* ctx, uint32_t tick)
{
    // This node didn't pong us back - remove from the table
    // Timers and node share same lookup key
    ((void)tick);
    ktable* table = (ktable*)ctx;
    ktable_remove_key(table, t->key);
    return 0;
}

int
ktable_timer_refresh(utimers* t, void* ctx, uint32_t tick)
{
    // Send some find nodes
    ktable* table = (ktable*)ctx;
    knodes* n;
    uint32_t alpha = 0;
    uint8_t id[65] = { 0x04 };

    for (int i = 0; i < KTABLE_N_NODES; i++) {
        urand(&id[1], 64);
        n = knodes_get(table->nodes, i);
        if (n) {
            table->settings.want_find(table, n, id, 65);
            if (++alpha >= table->settings.alpha) break;
        }
    }

    // Kick timer again
    utimers_start(table->timers, table->timerid, 0);
    return 0;
}
