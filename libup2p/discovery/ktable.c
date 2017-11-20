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

int
ktable_init(ktable* table, ktable_settings* settings, void* ctx)
{
    memset(table, 0, sizeof(ktable));
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
    utimers_deinit(table->timers, KTABLE_N_TIMERS);
    memset(table, 0, sizeof(ktable));
}

uint32_t
ktable_size(ktable* self)
{
    // TODO
    // return kh_size(self->nodes);
}

void
ktable_poll(ktable* self)
{
    utimers_poll(self->timers, KTABLE_N_TIMERS);
}

int
ktable_ping(
    ktable* self,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id)
{
    knodes* node = ktable_get(self, key);
    if (!node) {
        // table insert auto pings (we'll see if we need to change that)
        ktable_insert(self, key, ip, tcp, udp, id, NULL);
        return 0;
    } else {
        if (udp) node->udp = udp;
        if (tcp) node->tcp = tcp;
        return 0;
    }
}

int
ktable_pong(
    ktable* self,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id)

{
    knodes* node = ktable_get(self, key);
    if (node) {
        utimers_cancel(self->timers, node->key);
        if (ip) node->ip = ip;
        if (tcp) node->tcp = tcp;
        if (udp) node->udp = udp;
        if (id) node->nodeid = *id;
        return 0;
    } else {
        // unsolicited pong
        return -1;
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
ktable_get(ktable* self, ktable_key key)
{
    // TODO
    // khiter_t k = kh_get(knode_table, self->nodes, key);
    // return k == kh_end(self->nodes) ? NULL : &kh_val(self->nodes, k);
}

ktable_key
ktable_insert_rlp(ktable* table, ktable_key key, const urlp* rlp)
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
        return ktable_insert(table, key, ip, udp, tcp, &q, NULL);
    }
    return 0;
}

ktable_key
ktable_insert(
    ktable* self,
    ktable_key key,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta)
{
    // TODO
    ((void)meta);
    int absent;
    knodes* n = NULL;
    int k;
    if (ktable_size(self) < self->settings.size) {
        // k = kh_put(knode_table, self->nodes, key, &absent);
        // n = &kh_val(self->nodes, k);
        n->ip = ip;
        n->tcp = tcp;
        n->udp = udp;
        if (id) n->nodeid = *id;
        n->key = key;

        // Need devp2p hello to figure out if we like this node
        // This will probably change with introduction of topics in the
        // udp discovery protocol.
        //
        // The rlpx_io_discovery driver will mark this node as useless if
        // it doesn't like it - it will then be overwritten with other
        // nodes when state is set to false.
        //
        // Not investing much effort here.
        // TODO - state should be something like RLPX_STATE_WANT_PONG
        // On pong update recents and have discovery connect use that.
        // n->state = KNODE_STATE_PENDING;

        // Ping this node
        // self->settings.want_ping(self, n);

        // Start timer - if no pong remove from table
        n->key = key;
        utimers_insert(self->timers, n->key, ktable_timer_want_pong, self);
        utimers_start(self->timers, n->key, self->settings.pong_timeout);
        return 0;
    }
    return 0;
}

void
ktable_remove(ktable* self, ktable_key key)
{
    // TODO
    // khiter_t k = kh_get(knode_table, self->nodes, key);
    // if (!(k == kh_end(self->nodes))) kh_del(knode_table, self->nodes, k);
}

int
ktable_timer_want_pong(utimers* t, void* ctx, uint32_t tick)
{
    // This node didn't pong us back - remove from the table
    // Timers and node share same lookup key
    ((void)tick);
    ktable* table = (ktable*)ctx;
    ktable_remove(table, t->key);
    return 0;
}

int
ktable_timer_refresh(utimers* t, void* ctx, uint32_t tick)
{
    // Send some find nodes
    ktable* table = (ktable*)ctx;
    int k;
    knodes* n;
    uint8_t id[65] = { 0x04 };

    // TODO send find to alpha number of nodes
    // for (k = kh_begin(table->nodes); k != kh_end(table->nodes); k++) {
    //    if (kh_exist(table->nodes, k)) {
    //        n = &kh_val(table->nodes, k);
    //        urand(&id[1], 64);
    //        table->settings.want_find(table, n, id, 65);
    //    }
    //}
    // Kick timer again
    utimers_start(table->timers, table->timerid, 0);
    return 0;
}
