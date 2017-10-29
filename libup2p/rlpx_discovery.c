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

int
rlpx_discovery_recv(usys_sockaddr* ep, const uint8_t* b, uint32_t l)
{
    uecc_public_key pub;
    RLPX_DISCOVERY type;
    int err = -1;
    urlp* rlp;
    const urlp* crlp;

    // Parse (rlp is allocated on success - must free)
    if ((err = rlpx_discovery_parse(ep, b, l, &pub, (int*)&type, &rlp))) {
        return err;
    }

    crlp = rlp;
    if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_ping(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_pong(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_FIND) {
        err = rlpx_discovery_parse_find(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_NEIGHBOURS) {
        err = rlpx_discovery_parse_neighbours(ep, &crlp);
    } else {
        // error
    }

    // Free and return
    urlp_free(&rlp);
    return err;
}

// h256:32 + Signature:65 + type + RLP
int
rlpx_discovery_parse(usys_sockaddr* addr,
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
rlpx_discovery_parse_neighbours(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO - populate our node table
    urlp_foreach(*rlp, addr, rlpx_walk_neighbours);
    return 0;
}

void
rlpx_walk_neighbours(const urlp* rlp, int idx, void* ctx)
{
    ((void)ctx);
}
