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
#include "unonce.h"
#include "usys_log.h"
#include "usys_time.h"

// Private io callbacks (discv4)
int rlpx_io_on_erro_from(void* ctx);
int rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_io_on_recv_from(void* ctx, int err, uint8_t* b, uint32_t l);

// Private protocol callbacks
int rlpx_io_on_hello(void* ctx, const urlp* rlp);
int rlpx_io_on_disconnect(void* ctx, const urlp* rlp);
int rlpx_io_on_ping(void* ctx, const urlp* rlp);
int rlpx_io_on_pong(void* ctx, const urlp* rlp);
int rlpx_io_on_recv_auth(void* ctx, int err, uint8_t* b, uint32_t l);
int rlpx_io_on_recv_ack(void* ctx, int err, uint8_t* b, uint32_t l);

async_io_settings g_rlpx_disc_settings = {
    .on_accept = NULL,               //
    .on_connect = NULL,              //
    .on_erro = rlpx_io_on_erro_from, //
    .on_send = rlpx_io_on_send_to,   //
    .on_recv = rlpx_io_on_recv_from, //
};

// IO callback handlers
async_io_settings g_rlpx_io_settings = { //
    .on_accept = rlpx_io_on_accept,
    .on_connect = rlpx_io_on_connect,
    .on_erro = rlpx_io_on_erro,
    .on_send = rlpx_io_on_send,
    .on_recv = rlpx_io_on_recv
};

// TODO move these to rlpx_io and then compile unit tests.
rlpx_io*
rlpx_io_alloc(
    uecc_ctx* skey,
    const uint32_t* listen,
    async_io_settings* settings)
{
    rlpx_io* self = rlpx_malloc(sizeof(rlpx_io));
    if (self) rlpx_io_init_tcp(self, skey, listen, settings);
    return self;
}

void
rlpx_io_free(rlpx_io** p)
{
    rlpx_io* self = *p;
    *p = NULL;
    rlpx_io_deinit(self);
    rlpx_free(self);
}

void
rlpx_io_init(rlpx_io* io, uecc_ctx* s, const uint32_t* listen)
{
    // clear
    memset(io, 0, sizeof(rlpx_io));

    // Our static identity
    io->skey = s;

    // Create random epheremeral key
    uecc_key_init_new(&io->ekey);

    // update info
    io->listen_port = listen;
    uecc_qtob(&io->skey->Q, io->node_id, 65);

    // "virtual functions - want install"
    for (int32_t i = 0; i < RLPX_IO_MAX_PROTOCOL; i++) {
        io->protocols[i].ready = rlpx_io_default_on_ready;
        io->protocols[i].recv = rlpx_io_default_on_recv;
    }
}

void
rlpx_io_init_udp(
    rlpx_io* io,
    uecc_ctx* s,
    const uint32_t* listen,
    async_io_settings* settings)
{
    // init base
    rlpx_io_init(io, s, listen);
    // init io driver
    async_io_init_udp(&io->io, io, settings ? settings : &g_rlpx_disc_settings);
    rlpx_io_listen(io);
}

void
rlpx_io_init_tcp(
    rlpx_io* io,
    uecc_ctx* s,
    const uint32_t* listen,
    async_io_settings* settings)
{
    // Init base
    rlpx_io_init(io, s, listen);
    // io driver
    async_io_init(&io->io, io, settings ? settings : &g_rlpx_io_settings);
}

void
rlpx_io_deinit(rlpx_io* ch)
{
    uecc_key_deinit(&ch->ekey);
    for (uint32_t i = 0; i < RLPX_IO_MAX_PROTOCOL; i++) {
        if (ch->protocols[i].context) {
            ch->protocols[i].uninstall(&ch->protocols[i].context);
        }
    }
    if (ch->hs) rlpx_handshake_free(&ch->hs);
}

void
rlpx_io_nonce(rlpx_io* ch)
{
    unonce(ch->nonce.b);
}

int
rlpx_io_poll(rlpx_io** ch, uint32_t count, uint32_t ms)
{
    return async_io_poll_n((async_io**)ch, count, ms);
}

int
rlpx_io_listen(rlpx_io* io)
{
    return usys_listen_udp(&io->io.sock, *io->listen_port);
}

int
rlpx_io_connect(
    rlpx_io* ch,
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
rlpx_io_connect_enode(rlpx_io* ch, const char* enode)
{
    rlpx_node n;
    if (!rlpx_node_init_enode(&n, enode)) {
        return rlpx_io_connect_node(ch, &n);
    } else {
        return -1;
    }
}

int
rlpx_io_connect_node(rlpx_io* ch, const rlpx_node* n)
{
    ch->node = *n;
    return async_io_connect(&ch->io, n->ip_v4, n->port_tcp) < 0 ? -1 : 0;
}

int
rlpx_io_accept(rlpx_io* ch, const uecc_public_key* from)
{
    // TODO - this is a stub.
    if (ch->hs) rlpx_handshake_free(&ch->hs);
    ch->node.id = *from;
    ch->hs = rlpx_handshake_alloc(0, ch->skey, &ch->ekey, &ch->nonce, from);
    if (ch->hs) {
        async_io_accept(&ch->io); // stub
        ch->io.sock = 3;          // stub
        async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
        return rlpx_io_send(&ch->io);
    } else {
        return -1;
    }
}

int
rlpx_io_send_auth(rlpx_io* ch)
{

    if (ch->hs) rlpx_handshake_free(&ch->hs);
    ch->hs =
        rlpx_handshake_alloc(1, ch->skey, &ch->ekey, &ch->nonce, &ch->node.id);
    if (ch->hs) {
        usys_log("[OUT] (auth) size: %d", ch->hs->cipher_len);
        async_io_set_cb_recv(&ch->io, rlpx_io_on_recv_ack);
        async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
        return rlpx_io_send(&ch->io);
    } else {
        return -1;
    }
}

int
rlpx_io_send(async_io* io)
{
    int err = async_io_send(io);
    // TODO - breaks test (flushing io resets len)
    // if (!err) err = async_io_poll(io);
    return err;
}

int
rlpx_io_recv(rlpx_io* ch, const uint8_t* d, size_t l)
{
    int err = 0;
    uint16_t type;
    uint32_t sz;
    rlpx_io_protocol* p = NULL;
    urlp* rlp = NULL;
    while ((l) && (!err)) {
        sz = rlpx_frame_parse(&ch->x, d, l, &rlp);
        if (sz > 0) {
            if (sz <= l) {
                if (!urlp_idx_to_u16(rlp, 0, &type)) {
                    p = type < RLPX_IO_MAX_PROTOCOL ? &ch->protocols[type]
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

// h256:32 + Signature:65 + type + RLP
int
rlpx_io_recv_udp(rlpx_io* ch, const uint8_t* b, size_t l)
{
    int type, err;
    urlp *rlp, *list;
    rlpx_io_protocol* p = &ch->protocols[0];
    if (!(err = rlpx_io_parse_udp(b, l, &ch->node.id, &type, &rlp))) {
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
rlpx_io_recv_auth(rlpx_io* ch, const uint8_t* b, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt authentication packet (allocates rlp context)
    if ((err = rlpx_handshake_auth_recv(ch->hs, b, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_auth_install(ch->hs, &rlp))) {
        err = rlpx_handshake_secrets(
            ch->hs,
            0,
            &ch->x.emac,
            &ch->x.imac,
            &ch->x.aes_enc,
            &ch->x.aes_dec,
            &ch->x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_recv_ack(rlpx_io* ch, const uint8_t* ack, size_t l)
{
    int err = -1;
    urlp* rlp = NULL;

    // Decrypt authentication packet
    if ((err = rlpx_handshake_ack_recv(ch->hs, ack, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_ack_install(ch->hs, &rlp))) {
        err = rlpx_handshake_secrets(
            ch->hs,
            1,
            &ch->x.emac,
            &ch->x.imac,
            &ch->x.aes_enc,
            &ch->x.aes_dec,
            &ch->x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_on_recv_auth(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io* ch = (rlpx_io*)ctx;
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
    rlpx_io* ch = (rlpx_io*)ctx;
    if (!err) {
        if (!rlpx_io_recv_ack(ch, b, l)) {
            // TODO Free handshake?
            l -= ch->hs->cipher_remote_len;
            usys_log("[ IN] (ack) size: %d", ch->hs->cipher_remote_len);
            if (l) {
                if (rlpx_io_recv(ch, &b[ch->hs->cipher_remote_len], l)) {
                    usys_log_err("[ERR] %d", ch->io.sock);
                }
            }
            async_io_set_cb_recv(&ch->io, rlpx_io_on_recv);
            return ch->protocols[0].ready(ch->protocols[0].context);
        } else {
            usys_log_err("[ERR] socket %d (ack)", ch->io.sock);
            return -1;
        }
    } else {
        usys_log_err("[ERR] socket %d (ack)", ch->io.sock);
        return err;
    }
}

int
rlpx_io_on_accept(void* ctx)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    usys_log("p2p.accept %d", ch->io.sock);
    return 0;
}

int
rlpx_io_on_connect(void* ctx)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    return rlpx_io_send_auth(ch);
}

int
rlpx_io_on_erro(void* ctx)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    usys_log_err("[ERR] %d", ch->io.sock);
    return 0;
}

int
rlpx_io_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    ((void)b);
    ((void)l);
    if (!err) {
        return 0;
    } else {
        usys_log_err("[ERR] socket: %d", ch->io.sock);
        return -1;
    }
}

int
rlpx_io_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    if (!err) {
        return rlpx_io_recv(ch, b, l);
    } else {
        usys_log_err("[ERR] socket: %d", ch->io.sock);
        return -1;
    }
}

int
rlpx_io_on_erro_from(void* ctx)
{
    ((void)ctx);
    usys_log("[ IN] [UDP] error");
    return 0;
}

int
rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    ((void)ctx);
    ((void)err);
    ((void)b);
    ((void)l);
    usys_log("[ IN] [UDP] send");
    return 0;
}

int
rlpx_io_on_recv_from(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_io* self = ctx;
    if (!err) {
        err = rlpx_io_recv_udp(self, b, l);
    }
    usys_log("[ IN] [UDP] %S", err ? "recv (error)" : "recv");
    return err;
}

//
//
//
