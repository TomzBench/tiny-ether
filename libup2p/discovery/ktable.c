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

void
ktable_init(ktable* table)
{
    memset(table, 0, sizeof(ktable));
}

int
ktable_find_node(ktable* table, uecc_public_key* target, knode* node)
{
    uint32_t i = 0, c = KTABLE_SIZE;
    for (i = 0; i < c; i++) {
        if (memcmp(
                table->nodes[i].nodeid.data,
                target->data,
                sizeof(target->data))) {
            node = &table->nodes[i];
            return 0;
        }
    }
    return -1;
}

void
ktable_update_recent(ktable* table, knode* node)
{
    table->recents[2] = table->recents[1];
    table->recents[1] = table->recents[0];
    table->recents[0] = node;
}

int
ktable_node_add_rlp(ktable* table, const urlp* rlp)
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
        err = ktable_node_add(table, ip, udp, tcp, &q, NULL);
    }
    return err;
}

int
ktable_node_add(
    ktable* table,
    uint32_t ip,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta)
{
    knode* n;

    ((void)meta); // potential use in future

    // Seek a free slot in our table and populate
    n = ktable_node_get_id(table, NULL);
    if (n) {
        // Have a free slot to populate
        // memset(n->ip, 0, 16);
        // memcpy(n->ip, ip, iplen);
        // n->iplen = iplen;
        //*((uint32_t*)&n->ip) = ip;
        // n->iplen = 4;
        n->ip = ip;
        n->udp = udp;
        n->tcp = tcp;
        n->nodeid = *id;

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
        return 0;
    } else {
        // TODO Ping some nodes free some space
    }
    return -1;
}

knode*
ktable_node_get_id(ktable* table, const uecc_public_key* id)
{
    uint32_t c = KTABLE_SIZE;
    knode* seek = NULL;
    if (id) {
        for (uint32_t i = 0; i < c; i++) {
            seek = &table->nodes[i];
            if (seek->state && uecc_cmpq(&seek->nodeid, id)) return seek;
        }
    } else {
        for (uint32_t i = 0; i < c; i++) {
            seek = &table->nodes[i];
            if (!seek->state) return seek;
        }
    }
    // Arrive here didn't find what caller wants
    return NULL;
}
