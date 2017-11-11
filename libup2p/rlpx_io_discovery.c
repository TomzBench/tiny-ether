// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be state,
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

#include "rlpx_io_discovery.h"
#include "ukeccak256.h"
#include "urand.h"
#include "usys_log.h"
#include "usys_time.h"

void rlpx_walk_neighbours(const urlp* rlp, int idx, void* ctx);

void
rlpx_io_discovery_init(rlpx_io_discovery* self, rlpx_io_udp* base)
{
    memset(self, 0, sizeof(rlpx_io_discovery));
    self->base = base;
    base->rlpx.protocols[0].context = self;
    base->rlpx.protocols[0].ready = rlpx_io_discovery_ready;
    base->rlpx.protocols[0].recv = rlpx_io_discovery_recv;
    base->rlpx.protocols[0].uninstall = rlpx_io_discovery_uninstall;
}

int
rlpx_io_discovery_install(rlpx_io_udp* base)
{
    rlpx_io_discovery* self = base->rlpx.protocols[0].context
                                  ? NULL
                                  : rlpx_malloc(sizeof(rlpx_io_discovery));
    if (self) {
        rlpx_io_discovery_init(self, base);
        return 0;
    }
    return -1;
}

void
rlpx_io_discovery_uninstall(void** ptr_p)
{
    rlpx_io_discovery* ptr = *ptr_p;
    *ptr_p = NULL;
    rlpx_free(ptr);
}

rlpx_io_discovery*
rlpx_io_discovery_get_context(rlpx_io_udp* rlpx)
{
    return rlpx->rlpx.protocols[0].context;
}

void
rlpx_io_discovery_table_init(rlpx_io_discovery_table* table)
{
    memset(table, 0, sizeof(rlpx_io_discovery_table));
}

void
rlpx_io_discovery_endpoint_v4_init(
    rlpx_io_discovery_endpoint* ep,
    uint32_t ip,
    uint32_t udp,
    uint32_t tcp)
{
    memset(ep, 0, sizeof(rlpx_io_discovery_endpoint));
    *((uint32_t*)ep->ip) = ip;
    ep->iplen = 4;
    ep->udp = udp;
    ep->tcp = tcp;
}

void
rlpx_io_discovery_endpoint_v6_init(
    rlpx_io_discovery_endpoint* ep,
    uint8_t* ipv6,
    uint32_t udp,
    uint32_t tcp)
{
    memset(ep, 0, sizeof(rlpx_io_discovery_endpoint));
    memcpy(ep->ip, ipv6, 16);
    ep->iplen = 16;
    ep->udp = udp;
    ep->tcp = tcp;
}

int
rlpx_io_discovery_table_find_node(
    rlpx_io_discovery_table* table,
    uecc_public_key* target,
    rlpx_io_discovery_endpoint_node* node)
{
    uint32_t i = 0, c = RLPX_IO_DISCOVERY_TABLE_SIZE;
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
rlpx_io_discovery_table_update_recent(
    rlpx_io_discovery_table* table,
    rlpx_io_discovery_endpoint_node* node)
{
    table->recents[2] = table->recents[1];
    table->recents[1] = table->recents[0];
    table->recents[0] = node;
}

int
rlpx_io_discovery_table_node_add_rlp(
    rlpx_io_discovery_table* table,
    const urlp* rlp)
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
        err = rlpx_io_discovery_table_node_add(
            table, ipbuf, iplen, udp, tcp, &q, NULL);
    }
    return err;
}

int
rlpx_io_discovery_table_node_add(
    rlpx_io_discovery_table* table,
    uint8_t* ip,
    uint32_t iplen,
    uint32_t tcp,
    uint32_t udp,
    uecc_public_key* id,
    urlp* meta)
{
    rlpx_io_discovery_endpoint_node* n;

    ((void)meta); // potential use in future

    // Seek a free slot in our table and populate
    n = rlpx_io_discovery_table_node_get_id(table, NULL);
    if (n) {
        // Have a free slot to populate
        memset(n->ep.ip, 0, 16);
        memcpy(n->ep.ip, ip, iplen);
        n->ep.iplen = iplen;
        n->ep.udp = udp;
        n->ep.tcp = tcp;
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
        n->state = RLPX_STATE_PENDING;
        return 0;
    } else {
        // TODO Ping some nodes free some space
    }
    return -1;
}

rlpx_io_discovery_endpoint_node*
rlpx_io_discovery_table_node_get_id(
    rlpx_io_discovery_table* table,
    const uecc_public_key* id)
{
    uint32_t c = RLPX_IO_DISCOVERY_TABLE_SIZE;
    rlpx_io_discovery_endpoint_node* seek = NULL;
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

rlpx_io_discovery_endpoint_node*
rlpx_io_discovery_table_node_get_next(rlpx_io_discovery_table* table)
{
    static int spot = 0;
    if (spot >= RLPX_IO_DISCOVERY_TABLE_SIZE) spot = 0;
    while (spot++ < RLPX_IO_DISCOVERY_TABLE_SIZE) {
        if (table->nodes[spot].state) return &table->nodes[spot];
    }
    return NULL;
}

int
rlpx_io_discovery_ready(void* self)
{
    ((void)self);
    return 0;
}

int
rlpx_io_discovery_recv(void* ctx, const urlp* rlp)
{
    rlpx_io_discovery* self = ctx;
    // uecc_public_key pub;
    RLPX_DISCOVERY type = -1;
    uint16_t t;
    // rlpx_io_discovery_endpoint_node* node = NULL;
    rlpx_io_discovery_endpoint from, to;
    uecc_public_key target;
    uint32_t ts; // timestamp
    uint8_t buff32[32];
    int err = -1;
    const urlp* crlp = urlp_at(rlp, 1);
    if (!crlp) return -1;
    if (urlp_idx_to_u16(rlp, 0, &t)) return -1;
    type = (RLPX_DISCOVERY)t;

    memset(&from, 0, sizeof(rlpx_io_discovery_endpoint));
    memset(&to, 0, sizeof(rlpx_io_discovery_endpoint));

    // Update recently seen if this node is in our table
    // if (rlpx_io_discovery_table_find_node(&self->table, &pub, node)) {
    //    rlpx_io_discovery_table_update_recent(&self->table, node);
    //}

    if (type == RLPX_DISCOVERY_PING) {

        // Received a ping packet
        // send a pong on device io...
        err = rlpx_io_discovery_recv_ping(&crlp, buff32, &from, &to, &ts);
        if (!err) {
            return rlpx_io_discovery_send_pong(
                self,
                async_io_udp_from_ip(&self->base->io),
                async_io_udp_from_port(&self->base->io),
                &from,
                (h256*)buff32,
                ts);
        }
    } else if (type == RLPX_DISCOVERY_PONG) {

        // Received a pong packet
        err = rlpx_io_discovery_recv_pong(&crlp, &to, buff32, &ts);
    } else if (type == RLPX_DISCOVERY_FIND) {

        // Received request for our neighbours.
        // We send empty neighbours since we are not kademlia
        // We are leech looking for light clients servers
        err = rlpx_io_discovery_recv_find(&crlp, &target, &ts);
        if (!err) {
            return rlpx_io_discovery_send_neighbours(
                self,
                async_io_udp_from_ip(&self->base->io),
                async_io_udp_from_port(&self->base->io),
                &self->table,
                ts);
        }
    } else if (type == RLPX_DISCOVERY_NEIGHBOURS) {

        // Received some neighbours
        err = rlpx_io_discovery_recv_neighbours(&self->table, &crlp);
    } else {
        // error
    }

    return err;
}

int
rlpx_io_discovery_write(
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
rlpx_io_discovery_recv_endpoint(const urlp* rlp, rlpx_io_discovery_endpoint* ep)
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
rlpx_io_discovery_rlp_endpoint(const rlpx_io_discovery_endpoint* ep)
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
rlpx_io_discovery_recv_ping(
    const urlp** rlp,
    uint8_t* version32,
    rlpx_io_discovery_endpoint* from,
    rlpx_io_discovery_endpoint* to,
    uint32_t* timestamp)
{
    int err;
    uint32_t sz = 32, n = urlp_children(*rlp);
    if (n < 4) return -1;
    if ((!(err = urlp_idx_to_mem(*rlp, 0, version32, &sz))) && //
        (!(err = rlpx_io_discovery_recv_endpoint(urlp_at(*rlp, 1), from))) &&
        (!(err = rlpx_io_discovery_recv_endpoint(urlp_at(*rlp, 2), to))) &&
        (!(err = urlp_idx_to_u32(*rlp, 3, timestamp)))) {
        return err;
    }
    usys_log("[ IN] [UDP] (ping)");
    return err;
}

int
rlpx_io_discovery_recv_pong(
    const urlp** rlp,
    rlpx_io_discovery_endpoint* to,
    uint8_t* echo32,
    uint32_t* timestamp)
{
    int err;
    uint32_t sz = 32, n = urlp_children(*rlp);
    if (n < 4) return -1;
    if ((!(err = rlpx_io_discovery_recv_endpoint(urlp_at(*rlp, 0), to))) &&
        (!(err = urlp_idx_to_mem(*rlp, 1, echo32, &sz))) &&
        (!(err = urlp_idx_to_u32(*rlp, 2, timestamp)))) {
        return err;
    }
    usys_log("[ IN] [UDP] (pong)");
    return err;
}

int
rlpx_io_discovery_recv_find(const urlp** rlp, uecc_public_key* q, uint32_t* ts)
{
    int err = -1;
    uint32_t publen = 64, n = urlp_children(*rlp);
    uint8_t pub[65] = { 0x04 };
    if (n < 2) return err;
    if ((!(err = urlp_idx_to_mem(*rlp, 0, &pub[1], &publen))) &&
        //(!(err = uecc_btoq(pub, publen + 1, q))) && // TODO weird vals here
        (!(err = urlp_idx_to_u32(*rlp, 1, ts)))) {
        // TODO io errors
        // write tests to mimick send/recv
        // make all io writes sync or use queue
        // Trace mem bug
        // uint32_t l = 133;
        // char hex[l];
        // rlpx_node_bin_to_hex(pub, 65, hex, &l);
        // usys_log("[ IN] [UDP] recv find %s", hex);
        // q = NULL;
        err = 0;
    }
    usys_log("[ IN] [UDP] (find)");
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
rlpx_io_discovery_recv_neighbours(rlpx_io_discovery_table* t, const urlp** rlp)
{
    const urlp *n = urlp_at(*rlp, 0),         // get list of neighbours
        *ts = urlp_at(*rlp, 1);               // get timestamp
    ((void)ts);                               // TODO
    urlp_foreach(n, t, rlpx_walk_neighbours); // loop and add to table
    usys_log("[ IN] [UDP] (neighbours)");
    return 0;
}

void
rlpx_walk_neighbours(const urlp* rlp, int idx, void* ctx)
{
    // rlp.list(ipv(4|6),udp,tcp,nodeid)
    ((void)idx);
    rlpx_io_discovery_table* table = (rlpx_io_discovery_table*)ctx;
    rlpx_io_discovery_table_node_add_rlp(table, rlp);
}

int
rlpx_io_discovery_write_ping(
    uecc_ctx* skey,
    uint32_t ver,
    const rlpx_io_discovery_endpoint* ep_src,
    const rlpx_io_discovery_endpoint* ep_dst,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l)
{
    int err = -1;
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, urlp_item_u32(ver));
        urlp_push(rlp, rlpx_io_discovery_rlp_endpoint(ep_src));
        urlp_push(rlp, rlpx_io_discovery_rlp_endpoint(ep_dst));
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_io_discovery_write(skey, RLPX_DISCOVERY_PING, rlp, dst, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_io_discovery_write_pong(
    uecc_ctx* skey,
    const rlpx_io_discovery_endpoint* ep_to,
    h256* echo,
    uint32_t timestamp,
    uint8_t* dst,
    uint32_t* l)
{
    int err = -1;
    urlp* rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, rlpx_io_discovery_rlp_endpoint(ep_to));
        urlp_push(rlp, urlp_item_u8_arr(echo->b, 32));
        urlp_push(rlp, urlp_item_u32(timestamp));
        err = rlpx_io_discovery_write(skey, RLPX_DISCOVERY_PONG, rlp, dst, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_io_discovery_write_find(
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
        err = rlpx_io_discovery_write(skey, RLPX_DISCOVERY_FIND, rlp, b, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_io_discovery_write_neighbours(
    uecc_ctx* skey,
    rlpx_io_discovery_table* table,
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
        err =
            rlpx_io_discovery_write(skey, RLPX_DISCOVERY_NEIGHBOURS, rlp, b, l);
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_io_discovery_send_ping(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const rlpx_io_discovery_endpoint* ep_src,
    const rlpx_io_discovery_endpoint* ep_dst,
    uint32_t timestamp)
{
    int err;
    async_io* io = (async_io*)self->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_discovery_write_ping(
        self->base->rlpx.skey,
        4,
        ep_src,
        ep_dst,
        timestamp ? timestamp : usys_now(),
        async_io_buffer(io),
        len);
    usys_log("[OUT] [UDP] (ping) %d", *len);
    if (!err) err = rlpx_io_sendto_sync(&self->base->io, ip, port);
    return err;
}

int
rlpx_io_discovery_send_pong(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    const rlpx_io_discovery_endpoint* ep_to,
    h256* echo,
    uint32_t timestamp)
{
    int err;
    async_io* io = (async_io*)self->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_discovery_write_pong(
        self->base->rlpx.skey,
        ep_to,
        echo,
        timestamp ? timestamp : usys_now(),
        async_io_buffer(io),
        len);
    usys_log("[OUT] [UDP] (pong) %d", *len);
    if (!err) err = rlpx_io_sendto_sync(&self->base->io, ip, port);
    return err;
}

int
rlpx_io_discovery_send_find(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    uecc_public_key* nodeid,
    uint32_t timestamp)
{
    int err;
    async_io* io = (async_io*)&self->base->io;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_discovery_write_find(
        self->base->rlpx.skey,
        nodeid,
        timestamp ? timestamp : usys_now(),
        async_io_buffer(io),
        len);
    usys_log("[OUT] [UDP] (find) %d", *len);
    if (!err) err = rlpx_io_sendto_sync(&self->base->io, ip, port);
    return err;
}

int
rlpx_io_discovery_send_neighbours(
    rlpx_io_discovery* self,
    uint32_t ip,
    uint32_t port,
    rlpx_io_discovery_table* table,
    uint32_t timestamp)
{
    int err;
    async_io* io = (async_io*)self->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_discovery_write_neighbours(
        self->base->rlpx.skey,
        table,
        timestamp ? timestamp : usys_now(),
        async_io_buffer(io),
        len);
    usys_log("[OUT] [UDP] (neighbours) %d", *len);
    if (!err) err = rlpx_io_sendto_sync(&self->base->io, ip, port);
    return err;
}
