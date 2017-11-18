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

int ktable_timer_want_pong(usys_timer_key key, void* ctx, uint32_t tick);
int ktable_timer_refresh(usys_timer_key key, void* ctx, uint32_t tick);

int
ktable_init(ktable* table, ktable_settings* settings)
{
    memset(table, 0, sizeof(ktable));
    table->nodes = kh_init(knode_table);
    if (table->nodes) {
        table->settings = *settings;
        table->timerid = 1;
        kh_resize(knode_table, table->nodes, table->settings.size);
        usys_timers_init(&table->timers, table->settings.size + 1);
        usys_timers_insert(
            &table->timers,
            table->timerid,
            ktable_timer_refresh,
            table,
            table->settings.refresh);
        usys_timers_start(&table->timers, table->timerid, 0);
        return 0;
    }
    return -1;
}

void
ktable_deinit(ktable* table)
{
    kh_destroy(knode_table, table->nodes);
    usys_timers_deinit(&table->timers);
    memset(table, 0, sizeof(ktable));
}

uint32_t
ktable_size(ktable* self)
{
    return kh_size(self->nodes);
}

void
ktable_poll(ktable* self)
{
    usys_timers_poll(&self->timers);
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
    return -1;
}

int
ktable_pong(ktable* self, ktable_key key)
{
    knode* node = ktable_get(self, key);
    if (node) {
        if (node->timerid) usys_timers_cancel(&self->timers, node->timerid);
        return 0;
    } else {
        // unsolicited pong
        return -1;
    }
}

void
ktable_update_recent(ktable* table, knode* node)
{
    table->recents[2] = table->recents[1];
    table->recents[1] = table->recents[0];
    table->recents[0] = node;
}

knode*
ktable_get(ktable* self, ktable_key key)
{
    khiter_t k = kh_get(knode_table, self->nodes, key);
    return k == kh_end(self->nodes) ? NULL : &kh_val(self->nodes, k);
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
    ((void)meta);
    int absent;
    knode* n = NULL;
    khiter_t k;
    if (ktable_size(self) < self->settings.size) {
        k = kh_put(knode_table, self->nodes, key, &absent);
        n = &kh_val(self->nodes, k);
        n->ip = ip;
        n->tcp = tcp;
        n->udp = udp;
        n->key = key;
        if (id) n->nodeid = *id;

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
        n->state = KNODE_STATE_PENDING;

        // Ping this node
        self->settings.want_ping(self, n);

        // Start timer - if no pong remove from table
        n->timerid = key;
        usys_timers_insert(
            &self->timers,
            n->timerid,
            ktable_timer_want_pong,
            self,
            self->settings.pong_timeout);
        usys_timers_start(&self->timers, n->timerid, 0);
        return k;
    }
    return 0;
}

void
ktable_remove(ktable* self, ktable_key key)
{
    khiter_t k = kh_get(knode_table, self->nodes, key);
    if (!(k == kh_end(self->nodes))) kh_del(knode_table, self->nodes, k);
}

int
ktable_timer_want_pong(usys_timer_key key, void* ctx, uint32_t tick)
{
    // This node didn't pong us back - remove from the table
    // Timers and node share same lookup key
    ((void)tick);
    ktable* table = (ktable*)ctx;
    ktable_remove(table, key);
    return 0;
}

int
ktable_timer_refresh(usys_timer_key key, void* ctx, uint32_t tick)
{
    // Send some find nodes
    ktable* table = (ktable*)ctx;
    khiter_t k;
    knode* n;
    uint8_t id[64];
    for (k = kh_begin(table->nodes); k != kh_end(table->nodes); k++) {
        if (kh_exist(table->nodes, k)) {
            n = &kh_val(table->nodes, k);
            urand(id, sizeof(id));
            table->settings.want_find(table, n, id, 65);
            break;
        }
    }
    // Kick timer again
    usys_timers_start(&table->timers, table->timerid, 0);
    return 0;
}
