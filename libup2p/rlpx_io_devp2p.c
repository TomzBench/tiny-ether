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

#include "rlpx_io_devp2p.h"
#include "usys_log.h"
#include "usys_time.h"

const char* g_devp2p_hello_errors[] = { //
    "Disconnect requested.",            //
    "TCP error.",
    "Bad protocol.",
    "Remote has no use for us.",
    "Remote has to many peers.",
    "Already connected.",
    "Version mismatch.",
    "Bad public key.",
    "Remote shutting down.",
    "Unexpected public key.",
    "Connected to ourself!",
    "Timeout.",
    "Unknown reason."
};

int rlpx_io_devp2p_on_send_shutdown(
    void* ctx,
    int err,
    const uint8_t* b,
    uint32_t l);

void
rlpx_io_devp2p_init(rlpx_io_devp2p* self, rlpx_io* base)
{
    // erase
    memset(self, 0, sizeof(rlpx_io_devp2p));
    self->base = base;
    base->protocols[0].context = self;
    base->protocols[0].ready = rlpx_io_devp2p_ready;
    base->protocols[0].recv = rlpx_io_devp2p_recv;
    base->protocols[0].uninstall = rlpx_io_devp2p_uninstall;
}

int
rlpx_io_devp2p_install(rlpx_io* base)
{
    rlpx_io_devp2p* self =
        base->protocols[0].context ? NULL : rlpx_malloc(sizeof(rlpx_io_devp2p));
    if (self) {
        rlpx_io_devp2p_init(self, base);
        return 0;
    }
    return -1;
}

void
rlpx_io_devp2p_uninstall(void** ptr_p)
{
    rlpx_io_devp2p* ptr = *ptr_p;
    *ptr_p = NULL;
    rlpx_free(ptr);
}

int
rlpx_io_devp2p_recv(void* base, const urlp* rlp)
{
    int err = -1;
    const urlp *type = NULL, *body = NULL;
    rlpx_io_devp2p* self = ((rlpx_io*)base)->protocols[0].context;
    RLPX_DEVP2P_PROTOCOL_PACKET_TYPE package_type = DEVP2P_ERRO;
    if ((type = urlp_at(rlp, 0)) && (body = urlp_at(rlp, 1))) {

        package_type = urlp_as_u32(type);

        if (DEVP2P_HELLO == package_type) {
            err = rlpx_io_devp2p_recv_hello(self, body);
        } else if (DEVP2P_DISCONNECT == package_type) {
            err = rlpx_io_devp2p_recv_disconnect(self, body);
        } else if (DEVP2P_PING == package_type) {
            err = rlpx_io_devp2p_recv_ping(self, body);
            if (!err) rlpx_io_devp2p_send_pong(self);
        } else if (DEVP2P_PONG == package_type) {
            err = rlpx_io_devp2p_recv_pong(self, body);
        }
    }

    return err;
}

int
rlpx_io_devp2p_write(
    rlpx_coder* x,
    RLPX_DEVP2P_PROTOCOL_PACKET_TYPE type,
    urlp* rlp,
    uint8_t* out,
    uint32_t* outlen)
{
    int err = 0;
    uint32_t tmp = *outlen - 1;
    if (!*outlen) return -1;
    out[0] = type == DEVP2P_HELLO ? 0x80 : (uint8_t)type;
    if (rlp) {
        err = urlp_print(rlp, &out[1], &tmp);
        tmp++;
    } else {
        tmp = 1;
    }
    if (!err) err = rlpx_frame_write(x, 0, 0, out, tmp, out, outlen);
    return err;
}

int
rlpx_io_devp2p_write_hello(
    rlpx_coder* x,
    uint32_t port,
    const uint8_t* id,
    uint8_t* out,
    uint32_t* l)
{
    int err = -1;
    urlp *body = urlp_list(), *caps = urlp_list();

    // Create cababilities list (p2p/4)
    urlp_push(caps, urlp_push(urlp_item_str("p2p"), urlp_item_u32(4)));
    urlp_push(caps, urlp_push(urlp_item_str("les"), urlp_item_u32(1)));
    urlp_push(caps, urlp_push(urlp_item_str("pip"), urlp_item_u32(1)));

    // Create body list
    urlp_push(body, urlp_item_u32(RLPX_VERSION_P2P));
    urlp_push(body, urlp_item_str(RLPX_CLIENT_ID_STR));
    urlp_push(body, caps);
    urlp_push(body, urlp_item_u32(port));
    urlp_push(body, urlp_item_u8_arr(id, 64));

    // Encode
    err = rlpx_io_devp2p_write(x, DEVP2P_HELLO, body, out, l);
    urlp_free(&body);
    return err;
}

int
rlpx_io_devp2p_write_disconnect(
    rlpx_coder* x,
    RLPX_DEVP2P_DISCONNECT_REASON reason,
    uint8_t* out,
    uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    urlp_push(rlp, urlp_item_u32((uint32_t)reason));
    err = rlpx_io_devp2p_write(x, DEVP2P_DISCONNECT, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_devp2p_write_ping(rlpx_coder* x, uint8_t* out, uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    err = rlpx_io_devp2p_write(x, DEVP2P_PING, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_devp2p_write_pong(rlpx_coder* x, uint8_t* out, uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    err = rlpx_io_devp2p_write(x, DEVP2P_PONG, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_io_devp2p_ready(void* ctx)
{
    rlpx_io_devp2p* io = ctx;
    return rlpx_io_devp2p_send_hello(io);
}

int
rlpx_io_devp2p_on_recv(void* ctx, const urlp* rlp)
{
    return rlpx_io_devp2p_recv(ctx, rlp);
}

int
rlpx_io_devp2p_recv_hello(void* ctx, const urlp* rlp)
{
    const char* memptr;
    const uint8_t* pub;
    uint8_t pub_expect[65];
    uint32_t l, pip, les;
    rlpx_io_devp2p* ch = ctx;

    // Copy client string.
    rlpx_io_devp2p_client_id(rlp, &memptr, &l);
    if (l > RLPX_CLIENT_MAX_LEN) l = RLPX_CLIENT_MAX_LEN;
    memcpy(ch->client, memptr, l);

    // Copy listening port.
    rlpx_io_devp2p_listen_port(rlp, &ch->listen_port);

    // Check caps
    pip = rlpx_io_devp2p_capabilities(rlp, "pip", 1);
    les = rlpx_io_devp2p_capabilities(rlp, "les", 1);

    if ((rlp = urlp_at(rlp, 4)) &&                          //
        (pub = urlp_ref(rlp, &l)) &&                        //
        (l == 64) &&                                        //
        (!uecc_qtob(&ch->base->node.id, pub_expect, 65)) && //
        (!(memcmp(pub, &pub_expect[1], 64)))) {
        ch->base->ready = 1;
        usys_log("[ IN] (hello) %s pip:%d les:%d", ch->client, pip, les);
        return 0;
    } else {
        // Bad public key...
        usys_log_err("[ERR] Invalid \"hello\" - bad public key");
        ch->base->shutdown = 1;
        rlpx_io_close(ch->base);
        return -1;
    }
}

int
rlpx_io_devp2p_recv_disconnect(void* ctx, const urlp* rlp)
{
    rlpx_io_devp2p* ch = ctx;
    uint32_t reason = 12;
    int err = urlp_idx_to_u32(rlp, 0, &reason);
    usys_log(
        "[ IN] (disconnect) (%s)",
        (!err && (reason < 13)) ? g_devp2p_hello_errors[reason] : "unknown");
    rlpx_io_close(ch->base);
    return 0;
}

int
rlpx_io_devp2p_recv_ping(void* ctx, const urlp* rlp)
{
    ((void)rlp);
    ((void)ctx);
    // usys_log("[ IN] (ping)");
    return 0;
}

int
rlpx_io_devp2p_recv_pong(void* ctx, const urlp* rlp)
{
    ((void)rlp);
    rlpx_io_devp2p* ch = ctx;
    // usys_log("[ IN] (pong)");
    ch->latency = usys_tick() - ch->ping;
    return 0;
}

int
rlpx_io_devp2p_send_hello(rlpx_io_devp2p* ch)
{
    int err;
    async_io* io = (async_io*)ch->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_devp2p_write_hello(
        &ch->base->x,
        *ch->base->listen_port,
        &ch->base->node_id[1],
        async_io_buffer(io),
        len);
    if (!err) {
        usys_log("[OUT] (hello) size: %d", *len);
        return rlpx_io_send_sync(&ch->base->io);
    } else {
        return err;
    }
}

int
rlpx_io_devp2p_send_disconnect(
    rlpx_io_devp2p* ch,
    RLPX_DEVP2P_DISCONNECT_REASON reason)
{
    int err;
    async_io* io = (async_io*)ch->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_devp2p_write_disconnect(
        &ch->base->x, reason, async_io_buffer(io), len);
    if (!err) {
        usys_log("[OUT] (disconnect) size: %d", *len);
        async_io_on_send(&ch->base->io, rlpx_io_devp2p_on_send_shutdown);
        return rlpx_io_send_sync(&ch->base->io);
    } else {
        return err;
    }
}

int
rlpx_io_devp2p_send_ping(rlpx_io_devp2p* ch)
{
    int err;
    async_io* io = (async_io*)ch->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_devp2p_write_ping(&ch->base->x, async_io_buffer(io), len);
    if (!err) {
        ch->ping = usys_tick();
        // usys_log("[OUT] (ping) size: %d", *len);
        return rlpx_io_send_sync(&ch->base->io);
    } else {
        return err;
    }
}

int
rlpx_io_devp2p_send_pong(rlpx_io_devp2p* ch)
{
    int err;
    async_io* io = (async_io*)ch->base;
    uint32_t* len = async_io_buffer_length_pointer(io);
    async_io_len_reset(io);
    err = rlpx_io_devp2p_write_pong(&ch->base->x, async_io_buffer(io), len);
    if (!err) {
        // usys_log("[OUT] (pong) size: %d", *len);
        return rlpx_io_send_sync(&ch->base->io);
    } else {
        return err;
    }
}
int
rlpx_io_devp2p_on_send_shutdown(
    void* ctx,
    int err,
    const uint8_t* b,
    uint32_t l)
{
    rlpx_io* ch = (rlpx_io*)ctx;
    ((void)b);
    ((void)l);
    rlpx_io_close(ch);
    return err;
}

//
//
//
