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

#include "rlpx_discovery.h"
#include "ukeccak256.h"

void rlpx_walk_neighbours(const urlp* rlp, int idx, void* ctx);

void
rlpx_discovery_table_init(rlpx_discovery_table* table)
{
    memset(table, 0, sizeof(rlpx_discovery_table));
    table->recents[0] = &table->nodes[0];
    table->recents[1] = &table->nodes[1];
    table->recents[2] = &table->nodes[2];
}

int
rlpx_discovery_table_add_node_rlp(rlpx_discovery_table* table, const urlp* rlp)
{
    int err = 0;
    uint32_t n = urlp_siblings(rlp), udp, ip, tcp, publen = 64, iplen = 16;
    uint8_t ipbuf[iplen];
    uint8_t pub[publen + 1];
    uecc_public_key q;
    if (n < 4) return -1; /*!< invalid rlp */

    err |= urlp_idx_to_mem(rlp, 0, ipbuf, &iplen);
    err |= urlp_idx_to_u32(rlp, 0, &ip); // TODO ipv4 support only atm
    err |= urlp_idx_to_u32(rlp, 1, &udp);
    err |= urlp_idx_to_u32(rlp, 2, &tcp);
    err |= urlp_idx_to_mem(rlp, 3, &pub[1], &publen);
    if ((!err)) {
        pub[0] = 0x04;
        err = uecc_btoq(pub, publen + 1, &q);
        if (!err) {
            err =
                rlpx_discovery_table_add_node(table, ip, 4, udp, tcp, &q, NULL);
        }
    }
    return err;
}

int
rlpx_discovery_table_add_node(rlpx_discovery_table* table,
                              uint8_t ip,
                              uint32_t iplen,
                              uint32_t tcp,
                              uint32_t udp,
                              uecc_public_key* id,
                              urlp* meta)
{
    table->nodes[0].ep.ip[0] = 0; // TODO
    table->nodes[0].ep.udp = udp;
    table->nodes[0].ep.tcp = tcp;
    table->nodes[0].nodeid = *id;
    if (meta) {
    }
    return 0;
}

int
rlpx_discovery_recv(rlpx_discovery_table* t, const uint8_t* b, uint32_t l)
{
    uecc_public_key pub;
    RLPX_DISCOVERY type;
    int err = -1;
    urlp* rlp;
    const urlp* crlp;

    // Parse (rlp is allocated on success - must free)
    if ((err = rlpx_discovery_parse(t, b, l, &pub, (int*)&type, &rlp))) {
        return err;
    }

    crlp = rlp;
    if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_ping(NULL, &crlp);
    } else if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_pong(NULL, &crlp);
    } else if (type == RLPX_DISCOVERY_FIND) {
        err = rlpx_discovery_parse_find(NULL, &crlp);
    } else if (type == RLPX_DISCOVERY_NEIGHBOURS) {
        err = rlpx_discovery_recv_neighbours(t, &crlp);
    } else {
        // error
    }

    // Free and return
    urlp_free(&rlp);
    return err;
}

// h256:32 + Signature:65 + type + RLP
int
rlpx_discovery_parse(rlpx_discovery_table* t,
                     const uint8_t* b,
                     uint32_t l,
                     uecc_public_key* node_id,
                     int* type,
                     urlp** rlp)
{
    // Stack
    h256 hash, shash;
    int err;

    // Check len before parsing around
    if (l < (sizeof(h256) + 65 + 3)) return -1;

    // Check hash  hash = sha3(sig, type, rlp)
    ukeccak256((uint8_t*)&b[32], l - 32, hash.b, 32);
    if (memcmp(hash.b, b, 32)) return -1;

    // Recover signature from signed hash of type+rlp
    ukeccak256((uint8_t*)&b[32 + 65], l - (32 + 65), shash.b, 32);
    err = uecc_recover_bin(&b[32], shash.b, node_id);

    // Return OK
    *type = b[32 + 65];
    *rlp = urlp_parse(&b[32 + 65 + 1], l - (32 + 65 + 1));
    return 0;
}

int
rlpx_discovery_parse_ping(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO
    return 0;
}

int
rlpx_discovery_parse_pong(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO
    return 0;
}

int
rlpx_discovery_parse_find(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO - send neighbours
    return 0;
}

int
rlpx_discovery_recv_neighbours(rlpx_discovery_table* t, const urlp** rlp)
{
    // rlp.list(rlp.list(neighbours),timestamp)
    // rlp.list(rlp.list(neighbours),timestamp,a,b,c,d)
    // where neighbours = [ipv4|6,udp,tcp,nodeid]
    const urlp *n = urlp_at(*rlp, 0),         // get list of neighbours
        *ts = urlp_at(*rlp, 1);               // get timestamp
    ((void)ts);                               // TODO
    urlp_foreach(n, t, rlpx_walk_neighbours); // loop and add to table
    return 0;
}

void
rlpx_walk_neighbours(const urlp* rlp, int idx, void* ctx)
{
    // rlp.list(ipv(4|6),udp,tcp,nodeid)
    rlpx_discovery_table* table = (rlpx_discovery_table*)ctx;
    rlpx_discovery_table_add_node_rlp(table, rlp);
}
