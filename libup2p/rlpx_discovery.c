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
}

void
rlpx_discovery_endpoint_v4_init(
    rlpx_discovery_endpoint* ep,
    uint32_t ip,
    uint32_t udp,
    uint32_t tcp)
{
    memset(ep, 0, sizeof(rlpx_discovery_endpoint));
    *((uint32_t*)ep->ip) = ip;
    ep->iplen = 4;
    ep->udp = udp;
    ep->tcp = tcp;
}

void
rlpx_discovery_endpoint_v6_init(
    rlpx_discovery_endpoint* ep,
    uint8_t* ipv6,
    uint32_t udp,
    uint32_t tcp)
{
    memset(ep, 0, sizeof(rlpx_discovery_endpoint));
    memcpy(ep->ip, ipv6, 16);
    ep->iplen = 16;
    ep->udp = udp;
    ep->tcp = tcp;
}

int
rlpx_discovery_table_find_node(
    rlpx_discovery_table* table,
    uecc_public_key* target,
    rlpx_discovery_node* node)
{
    uint32_t i = 0, c = sizeof(table->nodes) - sizeof(rlpx_discovery_node);
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
rlpx_discovery_table_update_recent(
    rlpx_discovery_table* table,
    rlpx_discovery_node* node)
{
    table->recents[2] = table->recents[1];
    table->recents[1] = table->recents[0];
    table->recents[0] = node;
}

int
rlpx_discovery_table_add_node_rlp(rlpx_discovery_table* table, const urlp* rlp)
{
    int err = 0;
    uint32_t n = urlp_children(rlp), udp, tcp, publen = 64, iplen = 16;
    uint8_t ipbuf[iplen];
    uint8_t pub[65] = { 0x04 };
    uecc_public_key q;
    if (n < 4) return -1; /*!< invalid rlp */

    // short circuit bail. Arrive inside no errors
    if ((!(err = urlp_idx_to_mem(rlp, 0, ipbuf, &iplen))) &&
        (!(err = urlp_idx_to_u32(rlp, 1, &udp))) &&
        (!(err = urlp_idx_to_u32(rlp, 2, &tcp))) &&
        (!(err = urlp_idx_to_mem(rlp, 3, &pub[1], &publen))) &&
        (!(err = uecc_btoq(pub, publen + 1, &q)))) {
        err = rlpx_discovery_table_add_node(
            table, ipbuf, iplen, udp, tcp, &q, NULL);
    }
    return err;
}

int
rlpx_discovery_table_add_node(
    rlpx_discovery_table* table,
    uint8_t* ip,
    uint32_t iplen,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta)
{
    rlpx_discovery_node* n = &table->nodes[0];
    uint32_t i = 0, c = sizeof(table->nodes) - sizeof(rlpx_discovery_node);

    ((void)meta); // potential use in future

    // Seek a free slot in our table and populate
    for (i = 0; i < c; i++) {
        if ((n->useful == RLPX_USEFUL_FALSE) ||
            (n->useful == RLPX_USEFUL_FREE)) {
            memset(n->ep.ip, 0, 16);
            memcpy(n->ep.ip, ip, iplen);
            n->ep.udp = udp;
            n->ep.tcp = tcp;
            n->nodeid = *id;

            // Need devp2p hello to figure out if we like this node
            // This will probably change with introduction of topics in the
            // udp discovery protocol.
            //
            // The rlpx_discovery driver will mark this node as useless if
            // it doesn't like it - it will then be overwritten with other
            // nodes when usefulness is set to false.
            //
            // Not investing much effort here.
            n->useful = RLPX_USEFUL_PENDING;
            break;
        }
    }
    return 0;
}

int
rlpx_discovery_recv(rlpx_discovery_table* t, const uint8_t* b, uint32_t l)
{
    uecc_public_key pub;
    RLPX_DISCOVERY type;
    rlpx_discovery_node* node = NULL;
    rlpx_discovery_endpoint from, to;
    uecc_public_key target;
    uint32_t timestamp;
    uint8_t buff32[32];
    int err = -1;
    urlp* rlp;
    const urlp* crlp;

    // Parse (rlp is allocated on success - must free)
    if ((err = rlpx_discovery_parse(b, l, &pub, (int*)&type, &rlp))) {
        return err;
    }

    // Update recently seen if this node is in our table
    if (rlpx_discovery_table_find_node(t, &pub, node)) {
        rlpx_discovery_table_update_recent(t, node);
    }

    crlp = rlp;
    if (type == RLPX_DISCOVERY_PING) {

        // Received a ping packet
        // send a pong on device io...
        err = rlpx_discovery_parse_ping(&crlp, buff32, &from, &to, &timestamp);
    } else if (type == RLPX_DISCOVERY_PING) {

        // Received a pong packet
        err = rlpx_discovery_parse_pong(&crlp, &to, buff32, &timestamp);
    } else if (type == RLPX_DISCOVERY_FIND) {

        // Received request for our neighbours.
        // We send empty neighbours since we are not kademlia
        // We are leech looking for light clients servers
        err = rlpx_discovery_parse_find(&crlp, &target, &timestamp);
    } else if (type == RLPX_DISCOVERY_NEIGHBOURS) {

        // Received some neighbours
        err = rlpx_discovery_parse_neighbours(t, &crlp);
    } else {
        // error
    }

    // Free and return
    urlp_free(&rlp);
    return err;
}

// h256:32 + Signature:65 + type + RLP
int
rlpx_discovery_parse(
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
rlpx_discovery_write(
    uecc_ctx* key,
    RLPX_DISCOVERY type,
    const urlp* rlp,
    uint8_t* b,
    uint32_t* l)
{
    int err = -1;
    uecc_signature sig;
    h256 shash;
    uint32_t sz = *l - (32 + 65 + 1);

    // || packet-data
    err = urlp_print(rlp, &b[32 + 65 + 1], &sz);
    if (!err) {

        // || packet-type || packet-data
        b[32 + 65] = type;

        // sign(sha3(packet-type || packet-data)) || packet-type || packet-data
        ukeccak256((uint8_t*)&b[32 + 65], sz + 1, shash.b, 32);
        uecc_sign(key, shash.b, 32, &sig);
        uecc_sig_to_bin(&sig, &b[32]);

        // hash || sig || packet-type || packet-data
        ukeccak256((uint8_t*)&b[32], sz + 1 + 65, b, 32);

        *l = 32 + 65 + 1 + sz;
    }
    return err;
}

int
rlpx_discovery_parse_endpoint(const urlp* rlp, rlpx_discovery_endpoint* ep)
{
    int err;
    uint32_t n = urlp_children(rlp);
    if (n < 3) return -1;
    ep->iplen = sizeof(ep->ip);
    if ((!(err = urlp_idx_to_mem(rlp, 0, ep->ip, &ep->iplen))) &&
        (!(err = urlp_idx_to_u32(rlp, 1, &ep->udp))) &&
        (!(err = urlp_idx_to_u32(rlp, 2, &ep->tcp)))) {
        return err;
    }
    return err;
}
urlp*
rlpx_discovery_rlp_endpoint(const rlpx_discovery_endpoint* ep)
{
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, urlp_item_u8_arr(ep->ip, ep->iplen));
        urlp_push(rlp, urlp_item_u32(ep->udp));
        urlp_push(rlp, urlp_item_u32(ep->tcp));
        if (!(urlp_children(rlp) == 3)) urlp_free(&rlp);
    }
    return rlp;
}

int
rlpx_discovery_parse_ping(
    const urlp** rlp,
    uint8_t* version32,
    rlpx_discovery_endpoint* from,
    rlpx_discovery_endpoint* to,
    uint32_t* timestamp)
{
    int err;
    uint32_t sz = 32, n = urlp_children(*rlp);
    if (n < 4) return -1;
    if ((!(err = urlp_idx_to_mem(*rlp, 0, version32, &sz))) && //
        (!(err = rlpx_discovery_parse_endpoint(urlp_at(*rlp, 1), from))) &&
        (!(err = rlpx_discovery_parse_endpoint(urlp_at(*rlp, 2), to))) &&
        (!(err = urlp_idx_to_u32(*rlp, 3, timestamp)))) {
        return err;
    }
    return err;
}

int
rlpx_discovery_parse_pong(
    const urlp** rlp,
    rlpx_discovery_endpoint* to,
    uint8_t* echo32,
    uint32_t* timestamp)
{
    int err;
    uint32_t sz = 32, n = urlp_children(*rlp);
    if (n < 4) return -1;
    if ((!(err = rlpx_discovery_parse_endpoint(urlp_at(*rlp, 0), to))) &&
        (!(err = urlp_idx_to_mem(*rlp, 1, echo32, &sz))) &&
        (!(err = urlp_idx_to_u32(*rlp, 2, timestamp)))) {
        return err;
    }
    return err;
}

int
rlpx_discovery_parse_find(const urlp** rlp, uecc_public_key* q, uint32_t* ts)
{
    int err = -1;
    uint32_t publen = 64, n = urlp_children(*rlp);
    uint8_t pub[65] = { 0x04 };
    if (n < 2) return err;
    if ((!(err = urlp_idx_to_mem(*rlp, 0, &pub[1], &publen))) &&
        (!(err = uecc_btoq(pub, publen + 1, q))) &&
        (!(err = urlp_idx_to_u32(*rlp, 1, ts)))) {
        return err;
    }
    return err;
}

/**
 * @brief
 *
 * rlp.list(rlp.list(neighbours),timestamp)
 * rlp.list(rlp.list(neighbours),timestamp,a,b,c,d)
 * where neighbours = [ipv4|6,udp,tcp,nodeid]
 *
 * @param t
 * @param rlp
 *
 * @return
 */
int
rlpx_discovery_parse_neighbours(rlpx_discovery_table* t, const urlp** rlp)
{
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
    ((void)idx);
    rlpx_discovery_table* table = (rlpx_discovery_table*)ctx;
    rlpx_discovery_table_add_node_rlp(table, rlp);
}

int
rlpx_discovery_write_ping(
    uecc_ctx* skey,
    uint32_t ver,
    const rlpx_discovery_endpoint* ep_src,
    const rlpx_discovery_endpoint* ep_dst,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l)
{
    int err = -1;
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, urlp_item_u32(ver));
        urlp_push(rlp, rlpx_discovery_rlp_endpoint(ep_src));
        urlp_push(rlp, rlpx_discovery_rlp_endpoint(ep_dst));
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_discovery_write(skey, RLPX_DISCOVERY_PING, rlp, dst, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_discovery_write_pong(
    uecc_ctx* skey,
    const rlpx_discovery_endpoint* ep_to,
    h256* echo,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l)
{
    int err = -1;
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, rlpx_discovery_rlp_endpoint(ep_to));
        urlp_push(rlp, urlp_item_u8_arr(echo->b, 32));
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_discovery_write(skey, RLPX_DISCOVERY_PONG, rlp, dst, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_discovery_write_find(
    uecc_ctx* skey,
    uecc_public_key* nodeid,
    uint32_t timestamp,
    uint8_t* b,
    uint32_t* l)
{
    int err = -1;
    urlp* rlp = urlp_list();
    uint8_t pub[65];
    uecc_qtob(nodeid, pub, sizeof(pub));
    if (rlp) {
        urlp_push(rlp, urlp_item_u8_arr(&pub[1], 64));
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_discovery_write(skey, RLPX_DISCOVERY_FIND, rlp, b, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_discovery_write_neighbours(
    uecc_ctx* skey,
    rlpx_discovery_table* table,
    uint32_t timestamp,
    uint8_t* b,
    uint32_t* l)
{
    ((void)table); // we don't send anything
    int err = -1;
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, urlp_list()); // empty neighbours!
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_discovery_write(skey, RLPX_DISCOVERY_NEIGHBOURS, rlp, b, l);
        urlp_free(&rlp);
    }
    return err;
}
