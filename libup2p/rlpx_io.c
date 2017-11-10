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

#include "rlpx_io.h"
#include "usys_log.h"
#include "usys_time.h"

// Private io callbacks (discv4)
int rlpx_io_on_erro_from(void* ctx);
int rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_io_on_recv_from(void* ctx, int err, uint8_t* b, uint32_t l);
int rlpx_io_on_accept(void* ctx);
int rlpx_io_on_connect(void* ctx);
int rlpx_io_on_erro(void* ctx);
int rlpx_io_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_io_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);

// Private protocol callbacks
int rlpx_io_on_hello(void* ctx, const urlp* rlp);
int rlpx_io_on_disconnect(void* ctx, const urlp* rlp);
int rlpx_io_on_ping(void* ctx, const urlp* rlp);
int rlpx_io_on_pong(void* ctx, const urlp* rlp);
int rlpx_io_on_recv_auth(void* ctx, int err, uint8_t* b, uint32_t l);
int rlpx_io_on_recv_ack(void* ctx, int err, uint8_t* b, uint32_t l);

async_io_udp_settings g_rlpx_disc_settings = {
    .on_erro = rlpx_io_on_erro_from, //
    .on_send = rlpx_io_on_send_to,   //
    .on_recv = rlpx_io_on_recv_from, //
};

// IO callback handlers
async_io_tcp_settings g_rlpx_io_settings = { //
    .on_accept = rlpx_io_on_accept,
    .on_connect = rlpx_io_on_connect,
    .on_erro = rlpx_io_on_erro,
    .on_send = rlpx_io_on_send,
    .on_recv = rlpx_io_on_recv
};

// TODO move these to rlpx_io and then compile unit tests.
rlpx_io_tcp*
rlpx_io_alloc(uecc_ctx* skey, const uint32_t* listen)
{
    rlpx_io_tcp* self = rlpx_malloc(sizeof(rlpx_io_tcp));
    if (self) rlpx_io_tcp_init(self, skey, listen);
    return self;
}

void
rlpx_io_free(rlpx_io_tcp** p)
{
    rlpx_io_tcp* self = *p;
    *p = NULL;
    rlpx_io_tcp_deinit(self);
    rlpx_free(self);
}

void
rlpx_io_init(rlpx_io* rlpx, uecc_ctx* s, const uint32_t* listen)
{
    // clear
    memset(rlpx, 0, sizeof(rlpx_io));

    // Our static identity
    rlpx->skey = s;

    // Create random epheremeral key
    uecc_key_init_new(&rlpx->ekey);

    // update info
    rlpx->listen_port = listen;
    uecc_qtob(&rlpx->skey->Q, rlpx->node_id, 65);

    // "virtual functions - want install"
    for (int32_t i = 0; i < RLPX_IO_MAX_PROTOCOL; i++) {
        rlpx->protocols[i].ready = rlpx_io_default_on_ready;
        rlpx->protocols[i].recv = rlpx_io_default_on_recv;
    }
}

void
rlpx_io_udp_init(rlpx_io_udp* io, uecc_ctx* s, const uint32_t* listen)
{
    // init base
    rlpx_io_init(&io->rlpx, s, listen);
    // init io driver
    async_io_udp_init(&io->io, &g_rlpx_disc_settings, io);
    async_io_udp_listen(&io->io, *listen);
}

void
rlpx_io_tcp_init(rlpx_io_tcp* io, uecc_ctx* s, const uint32_t* listen)
{
    // Init base
    rlpx_io_init(&io->rlpx, s, listen);
    // io driver
    async_io_tcp_init(&io->io, &g_rlpx_io_settings, io);
}

void
rlpx_io_tcp_deinit(rlpx_io_tcp* tcp)
{
    rlpx_io_deinit(&tcp->rlpx);
    async_io_tcp_deinit(&tcp->io);
}

void
rlpx_io_udp_deinit(rlpx_io_udp* udp)
{
    rlpx_io_deinit(&udp->rlpx);
    async_io_udp_deinit(&udp->io);
}

void
rlpx_io_deinit(rlpx_io* rlpx)
{
    uecc_key_deinit(&rlpx->ekey);
    for (uint32_t i = 0; i < RLPX_IO_MAX_PROTOCOL; i++) {
        if (rlpx->protocols[i].context) {
            rlpx->protocols[i].uninstall(&rlpx->protocols[i].context);
        }
    }
    if (rlpx->hs) rlpx_handshake_free(&rlpx->hs);
    memset(rlpx, 0, sizeof(rlpx_io));
}

void
rlpx_io_refresh(rlpx_io* rlpx)
{
    rlpx->error = rlpx->shutdown = rlpx->ready = 0;
    rlpx_node_deinit(&rlpx->node);
    if (rlpx->hs) rlpx_handshake_free(&rlpx->hs);
}

int
rlpx_io_poll(rlpx_io_tcp** ch, uint32_t count, uint32_t ms)
{
    return async_io_poll_n((async_io**)ch, count, ms);
}

int
rlpx_io_connect(
    rlpx_io_tcp* ch,
    const uecc_public_key* to,
    uint32_t ip,
    uint32_t tcp)
{
    const char* host = usys_ntoa(ip);
    return rlpx_io_connect_host(ch, to, host, usys_ntohl(tcp));
}

int
rlpx_io_connect_host(
    rlpx_io_tcp* ch,
    const uecc_public_key* to,
    const char* host,
    uint32_t tcp)
{
    rlpx_node n;
    if (!rlpx_node_init(&n, to, host, tcp, 0)) {
        return rlpx_io_connect_node(ch, &n);
    } else {
        return -1;
    }
}

int
rlpx_io_connect_enode(rlpx_io_tcp* ch, const char* enode)
{
    rlpx_node n;
    if (!rlpx_node_init_enode(&n, enode)) {
        return rlpx_io_connect_node(ch, &n);
    } else {
        return -1;
    }
}

int
rlpx_io_connect_node(rlpx_io_tcp* ch, const rlpx_node* n)
{
    ch->rlpx.node = *n;
    return async_io_tcp_connect(&ch->io, n->ip_v4, n->port_tcp) < 0 ? -1 : 0;
}

int
rlpx_io_accept(rlpx_io_tcp* ch, const uecc_public_key* from)
{
    // TODO - this is a stub.
    if (ch->rlpx.hs) rlpx_handshake_free(&ch->rlpx.hs);
    ch->rlpx.node.id = *from;
    ch->rlpx.hs = rlpx_handshake_alloc(
        0, ch->rlpx.skey, &ch->rlpx.ekey, &ch->rlpx.nonce, from);
    if (ch->rlpx.hs) {
        async_io_tcp_accept(&ch->io); // stub
        ch->io.base.sock = 3;         // stub
        async_io_memcpy(
            &ch->io.base, 0, ch->rlpx.hs->cipher, ch->rlpx.hs->cipher_len);
        return rlpx_io_send_sync(&ch->io);
    } else {
        return -1;
    }
}

int
rlpx_io_send_auth(rlpx_io_tcp* ch)
{

    if (ch->rlpx.hs) rlpx_handshake_free(&ch->rlpx.hs);
    ch->rlpx.hs = rlpx_handshake_alloc(
        1, ch->rlpx.skey, &ch->rlpx.ekey, &ch->rlpx.nonce, &ch->rlpx.node.id);
    if (ch->rlpx.hs) {
        usys_log("[OUT] (auth) size: %d", ch->rlpx.hs->cipher_len);
        async_io_tcp_set_cb_recv(&ch->io, rlpx_io_on_recv_ack);
        async_io_memcpy(
            &ch->io.base, 0, ch->rlpx.hs->cipher, ch->rlpx.hs->cipher_len);
        return rlpx_io_send_sync(&ch->io);
    } else {
        return -1;
    }
}

int
rlpx_io_send(async_io_tcp* io)
{
    int err = async_io_tcp_send(io);
    // Need queue for async
    // TODO - breaks test (flushing io resets len)
    // if (!err) err = async_io_tcp_poll(io);
    return err;
}

int
rlpx_io_send_sync(async_io_tcp* tcp)
{
    int err = 0;
    async_io* io = (async_io*)tcp;
    if (!(async_io_has_sock(io))) return err;
    while ((async_io_state_send(io)) && (!err)) {
        usys_msleep(200);
        err = async_io_tcp_poll(tcp);
    }
    err = async_io_tcp_send(tcp);
    while ((async_io_state_send(io)) && (!err)) {
        usys_msleep(200);
        err = async_io_tcp_poll(tcp);
    }
    return err;
}

int
rlpx_io_sendto(async_io_udp* io, uint32_t ip, uint32_t port)
{
    int err = async_io_udp_send(io, ip, port);
    // Need queue for async
    // TODO - breaks test (flushing io resets len)
    // if (!err) err = async_io_udp_poll(io);
    return err;
}

int
rlpx_io_sendto_sync(async_io_udp* udp, uint32_t ip, uint32_t port)
{
    int err = -1;
    async_io* io = (async_io*)udp;
    if (!(async_io_has_sock(io))) return err;
    while (async_io_state_send(io)) {
        usys_msleep(20);
        async_io_poll(io);
    }
    err = async_io_udp_send(udp, ip, port);
    if (!err) {
        while (async_io_state_send(io)) {
            usys_msleep(20);
            async_io_poll(io);
        }
    }
    return err;
}

int
rlpx_io_recv(rlpx_io_tcp* ch, const uint8_t* d, size_t l)
{
    int err = 0;
    uint16_t type;
    uint32_t sz;
    rlpx_io_protocol* p = NULL;
    urlp* rlp = NULL;
    while ((l) && (!err)) {
        sz = rlpx_frame_parse(&ch->rlpx.x, d, l, &rlp);
        if (sz > 0) {
            if (sz <= l) {
                if (!urlp_idx_to_u16(rlp, 0, &type)) {
                    p = type < RLPX_IO_MAX_PROTOCOL ? &ch->rlpx.protocols[type]
                                                    : NULL;
                    err = p ? p->recv(ch, urlp_at(rlp, 1)) : -1;
                }
                d += sz;
                l -= sz;
            } else {
                err = -1;
            }
            urlp_free(&rlp);
        } else {
            err = -1;
        }
    }
    return err;
}

int
rlpx_io_parse_udp(
    const uint8_t* b,
    uint32_t len,
    uecc_public_key* node_id,
    int* type,
    urlp** rlp)
{
    // Stack
    h256 hash, shash;
    int err;

    // Check len before parsing around
    if (len < (sizeof(h256) + 65 + 3)) return -1;

    // Check hash  hash = sha3(sig, type, rlp)
    ukeccak256((uint8_t*)&b[32], len - 32, hash.b, 32);
    if (memcmp(hash.b, b, 32)) return -1;

    // Recover signature from signed hash of type+rlp
    ukeccak256((uint8_t*)&b[32 + 65], len - (32 + 65), shash.b, 32);
    err = uecc_recover_bin(&b[32], shash.b, node_id);

    // Return OK
    *type = b[32 + 65];
    *rlp = urlp_parse(&b[32 + 65 + 1], len - (32 + 65 + 1));
    return 0;
}

// h256:32 + Signature:65 + type + RLP
int
rlpx_io_recv_udp(rlpx_io_udp* ch, const uint8_t* b, size_t l)
{
    int type, err;
    urlp *rlp, *list;
    rlpx_io_protocol* p = &ch->rlpx.protocols[0];
    if (!(err = rlpx_io_parse_udp(b, l, &ch->rlpx.node.id, &type, &rlp))) {
        // We wrap the packet type and body into a list
        // type,[body]  -- per wire specification
        // [type,[body]] - per our implementation after wire for unified handler
        list = urlp_list();
        if (list) {
            urlp_push(list, urlp_item_u16(type));
            urlp_push(list, rlp);
            err = p->recv(p->context, list);
            urlp_free(&list);
        } else {
            urlp_free(&rlp);
        }
        return err;
    }
    return err;
}

int
rlpx_io_recv_auth(rlpx_io_tcp* ch, const uint8_t* b, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt authentication packet (allocates rlp context)
    if ((err = rlpx_handshake_auth_recv(ch->rlpx.hs, b, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_auth_install(ch->rlpx.hs, &rlp))) {
        err = rlpx_handshake_secrets(
            ch->rlpx.hs,
            0,
            &ch->rlpx.x.emac,
            &ch->rlpx.x.imac,
            &ch->rlpx.x.aes_enc,
            &ch->rlpx.x.aes_dec,
            &ch->rlpx.x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_recv_ack(rlpx_io_tcp* ch, const uint8_t* ack, size_t l)
{
    int err = -1;
    urlp* rlp = NULL;

    // Decrypt authentication packet
    if ((err = rlpx_handshake_ack_recv(ch->rlpx.hs, ack, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_ack_install(ch->rlpx.hs, &rlp))) {
        err = rlpx_handshake_secrets(
            ch->rlpx.hs,
            1,
            &ch->rlpx.x.emac,
            &ch->rlpx.x.imac,
            &ch->rlpx.x.aes_enc,
            &ch->rlpx.x.aes_dec,
            &ch->rlpx.x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_on_recv_auth(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    if (!err) {
        usys_log("[ IN] (auth) size: %d", l);
        return rlpx_io_recv_auth(ch, b, l);
    } else {
        return err;
    }
}

int
rlpx_io_on_recv_ack(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    if (!err) {
        if (!rlpx_io_recv_ack(ch, b, l)) {
            // TODO Free handshake?
            l -= ch->rlpx.hs->cipher_remote_len;
            usys_log("[ IN] (ack) size: %d", ch->rlpx.hs->cipher_remote_len);
            if (l) {
                if (rlpx_io_recv(ch, &b[ch->rlpx.hs->cipher_remote_len], l)) {
                    usys_log_err("[ERR] %d", ch->io.base.sock);
                }
            }
            async_io_tcp_set_cb_recv(&ch->io, rlpx_io_on_recv);
            return ch->rlpx.protocols[0].ready(ch->rlpx.protocols[0].context);
        } else {
            usys_log_err("[ERR] socket %d (ack)", ch->io.base.sock);
            return -1;
        }
    } else {
        usys_log_err("[ERR] socket %d (ack)", ch->io.base.sock);
        return err;
    }
}

int
rlpx_io_on_accept(void* ctx)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    usys_log("p2p.accept %d", ch->io.base.sock);
    return 0;
}

int
rlpx_io_on_connect(void* ctx)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    return rlpx_io_send_auth(ch);
}

int
rlpx_io_on_erro(void* ctx)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    usys_log_err("[ERR] %d", ch->io.base.sock);
    rlpx_io_error_set(&ch->rlpx, 1);
    async_io_close((async_io*)ch);
    return 0;
}

int
rlpx_io_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    ((void)b);
    ((void)l);
    if (!err) {
        return 0;
    } else {
        usys_log_err("[ERR] socket: %d", ch->io.base.sock);
        return -1;
    }
}

int
rlpx_io_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io_tcp* ch = (rlpx_io_tcp*)ctx;
    if (!err) {
        return rlpx_io_recv(ch, b, l);
    } else {
        usys_log_err("[ERR] socket: %d", ch->io.base.sock);
        return -1;
    }
}

int
rlpx_io_on_erro_from(void* ctx)
{
    ((void)ctx);
    usys_log("[ IN] [UDP] error %d", ((async_io*)ctx)->sock);
    return 0;
}

int
rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    ((void)ctx);
    ((void)err);
    ((void)b);
    ((void)l);
    if (err) usys_log("[OUT] [UDP] %s", "send (error)");
    return 0;
}

int
rlpx_io_on_recv_from(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io_udp* self = ctx;
    if (!err) err = rlpx_io_recv_udp(self, b, l);

    if (err) usys_log("[ IN] [UDP] %s", "recv (error)");
    return err;
}

//
//
//
