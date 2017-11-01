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

// Private io callbacks (devp2p)
int rlpx_io_on_accept(void* ctx);
int rlpx_io_on_connect(void* ctx);
int rlpx_io_on_erro(void* ctx);
int rlpx_io_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_io_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);

// Private io callbacks (discv4)
int rlpx_io_on_erro_from(void* ctx);
int rlpx_io_on_send_to(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_io_on_recv_from(void* ctx, int err, uint8_t* b, uint32_t l);

// Private protocol callbacks
int rlpx_io_on_hello(void* ctx, const urlp* rlp);
int rlpx_io_on_disconnect(void* ctx, const urlp* rlp);
int rlpx_io_on_ping(void* ctx, const urlp* rlp);
int rlpx_io_on_pong(void* ctx, const urlp* rlp);
int rlpx_io_on_send_shutdown(void* ctx, int err, const uint8_t* b, uint32_t l);
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

// Protocol callback handlers
rlpx_devp2p_protocol_settings g_devp2p_settings = { //
    .on_hello = rlpx_io_on_hello,
    .on_disconnect = rlpx_io_on_disconnect,
    .on_ping = rlpx_io_on_ping,
    .on_pong = rlpx_io_on_pong
};

rlpx_io*
rlpx_io_alloc(uecc_ctx* skey, const uint32_t* listen)
{
    rlpx_io* ch = rlpx_malloc(sizeof(rlpx_io));
    if (ch) {
        rlpx_io_init(ch, skey, listen);
    }
    return ch;
}

void
rlpx_io_free(rlpx_io** ch_p)
{
    rlpx_io* ch = *ch_p;
    *ch_p = NULL;
    rlpx_io_deinit(ch);
    rlpx_free(ch);
}

int
rlpx_io_init_discv4(rlpx_io* io, uecc_ctx* s, const uint32_t* listen)
{
    rlpx_io_init(io, s, listen);
    async_io_init(&io->io, io, &g_rlpx_disc_settings);
    rlpx_io_listen(io);
    return 0;
}

int
rlpx_io_init_devp2p(rlpx_io* io, uecc_ctx* s, const uint32_t* listen)
{
    rlpx_io_init(io, s, listen);
    async_io_init(&io->io, io, &g_rlpx_io_settings);
    return 0;
}

int
rlpx_io_init(rlpx_io* io, uecc_ctx* s, const uint32_t* listen)
{
    // clean mem
    memset(io, 0, sizeof(rlpx_io));

    // Our static identity
    io->skey = s;

    // Create random epheremeral key
    uecc_key_init_new(&io->ekey);

    // update info
    io->listen_port = listen;
    uecc_qtob(&io->skey->Q, io->node_id, 65);

    // Install protocols
    rlpx_devp2p_protocol_init(&io->devp2p, &g_devp2p_settings, io);
    // io->protocols[0] = (rlpx_protocol*)&io->devp2p;

    return 0;
}

void
rlpx_io_deinit(rlpx_io* ch)
{
    uecc_key_deinit(&ch->ekey);
    rlpx_devp2p_protocol_deinit(&ch->devp2p);
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
rlpx_io_send_hello(rlpx_io* ch)
{
    int err;
    async_io_set_cb_recv(&ch->io, rlpx_io_on_recv);
    ch->io.len = sizeof(ch->io.b);
    err = rlpx_devp2p_protocol_write_hello(
        &ch->x, *ch->listen_port, &ch->node_id[1], ch->io.b, &ch->io.len);
    if (!err) {
        usys_log("[OUT] (hello) size: %d", ch->io.len);
        return rlpx_io_send(&ch->io);
    } else {
        return err;
    }
}

int
rlpx_io_send_disconnect(rlpx_io* ch, RLPX_DEVP2P_DISCONNECT_REASON reason)
{
    int err;
    ch->io.len = sizeof(ch->io.b);
    err = rlpx_devp2p_protocol_write_disconnect(
        &ch->x, reason, ch->io.b, &ch->io.len);
    if (!err) {
        usys_log("[OUT] (disconnect) size: %d", ch->io.len);
        async_io_set_cb_send(&ch->io, rlpx_io_on_send_shutdown);
        return rlpx_io_send(&ch->io);
    } else {
        return err;
    }
}

int
rlpx_io_send_ping(rlpx_io* ch)
{
    int err;
    ch->io.len = sizeof(ch->io.b);
    err = rlpx_devp2p_protocol_write_ping(&ch->x, ch->io.b, &ch->io.len);
    if (!err) {
        ch->devp2p.ping = usys_now();
        usys_log("[OUT] (ping) size: %d", ch->io.len);
        return rlpx_io_send(&ch->io);
    } else {
        return err;
    }
}

int
rlpx_io_send_pong(rlpx_io* ch)
{
    int err;
    ch->io.len = sizeof(ch->io.b);
    err = rlpx_devp2p_protocol_write_pong(&ch->x, ch->io.b, &ch->io.len);
    if (!err) {
        usys_log("[OUT] (pong) size: %d", ch->io.len);
        return rlpx_io_send(&ch->io);
    } else {
        return err;
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
    urlp* rlp = NULL;
    // rlpx_devp2p_protocol* p;
    while ((l) && (!err)) {
        sz = rlpx_frame_parse(&ch->x, d, l, &rlp);
        if (sz > 0) {
            if (sz <= l) {
                if (!urlp_idx_to_u16(rlp, 0, &type)) {
                    if (type == 0) {
                        ch->devp2p.recv(&ch->devp2p, urlp_at(rlp, 1));
                    }
                    // p = (type >= 0 && type < 2) ? ch->protocols[type] : NULL;
                    // err = p ? p->recv(p, urlp_at(rlp, 1)) : -1;
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
    rlpx_io* io = (rlpx_io*)ctx;
    ((void)io);
    ((void)err);
    ((void)b);
    ((void)l);
    usys_log("[ IN] [UDP] hit");
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
rlpx_io_on_send_shutdown(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    ((void)b);
    ((void)l);
    ch->shutdown = 1;
    async_io_close(&ch->io);
    return err;
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
            return rlpx_io_send_hello(ch);
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
rlpx_io_on_hello(void* ctx, const urlp* rlp)
{
    const char* memptr;
    const uint8_t* pub;
    uint8_t pub_expect[65];
    uint32_t l;
    rlpx_io* ch = ctx;

    usys_log("[ IN] (hello)");

    // Copy client string.
    rlpx_devp2p_protocol_client_id(rlp, &memptr, &l);
    memcpy(
        ch->devp2p.client,
        memptr,
        l < RLPX_CLIENT_MAX_LEN ? l : RLPX_CLIENT_MAX_LEN);

    // Copy listening port.
    rlpx_devp2p_protocol_listen_port(rlp, &ch->devp2p.listen_port);

    // TODO - Check caps

    if ((rlp = urlp_at(rlp, 4)) &&                    //
        (pub = urlp_ref(rlp, &l)) &&                  //
        (l == 64) &&                                  //
        (!uecc_qtob(&ch->node.id, pub_expect, 65)) && //
        (!(memcmp(pub, &pub_expect[1], 64)))) {
        ch->ready = 1;
    } else {
        // Bad public key...
        usys_log_err("[ERR] Invalid \"hello\" - bad public key");
        ch->shutdown = 1;
        async_io_close(&ch->io);
    }

    return 0;
}

int
rlpx_io_on_disconnect(void* ctx, const urlp* rlp)
{
    rlpx_io* ch = ctx;
    ((void)ch);
    ((void)rlp); // TODO
    usys_log("[ IN] (disconnect)");
    return 0;
}

int
rlpx_io_on_ping(void* ctx, const urlp* rlp)
{
    ((void)rlp);
    rlpx_io* ch = ctx;
    usys_log("[ IN] (ping)");
    rlpx_io_send_pong(ch);
    return 0;
}

int
rlpx_io_on_pong(void* ctx, const urlp* rlp)
{
    ((void)rlp);
    rlpx_io* ch = ctx;
    usys_log("[ IN] (pong)");
    ch->devp2p.latency = usys_now() - ch->devp2p.ping;
    return 0;
}

//
//
//
