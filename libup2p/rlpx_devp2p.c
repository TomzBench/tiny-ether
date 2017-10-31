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

#include "rlpx_devp2p.h"

int rlpx_devp2p_protocol_recv(rlpx_protocol*, const urlp* rlp);

rlpx_devp2p_protocol*
rlpx_devp2p_protocol_alloc(
    const rlpx_devp2p_protocol_settings* settings,
    void* ctx)
{
    rlpx_devp2p_protocol* self = rlpx_malloc(sizeof(rlpx_devp2p_protocol));
    if (self) {
        rlpx_devp2p_protocol_init(self, settings, ctx);
    }
    return self;
}

void
rlpx_devp2p_protocol_free(rlpx_devp2p_protocol** self_p)
{
    rlpx_devp2p_protocol* self = *self_p;
    *self_p = NULL;
    rlpx_devp2p_protocol_deinit(self);
    rlpx_free(self);
}

void
rlpx_devp2p_protocol_init(
    rlpx_devp2p_protocol* self,
    const rlpx_devp2p_protocol_settings* settings,
    void* ctx)
{
    // Iniitlize base class.
    rlpx_protocol_init(&self->base, 0, "p2p", ctx);

    // Override recv method.
    self->base.recv = rlpx_devp2p_protocol_recv;

    // Install protocol specific handlers.
    self->settings = settings;
}

void
rlpx_devp2p_protocol_deinit(rlpx_devp2p_protocol* self)
{
    // Deinitialize base class
    rlpx_protocol_deinit(&self->base);
}

int
rlpx_devp2p_protocol_recv(rlpx_protocol* base, const urlp* rlp)
{
    int err = -1;
    const urlp *type = NULL, *body = NULL;
    rlpx_devp2p_protocol* self = (rlpx_devp2p_protocol*)base;
    RLPX_DEVP2P_PROTOCOL_PACKET_TYPE package_type = DEVP2P_ERRO;
    if ((type = urlp_at(rlp, 0)) && (body = urlp_at(rlp, 1))) {

        package_type = urlp_as_u32(type);

        if (DEVP2P_HELLO == package_type) {
            err = self->settings->on_hello(self->base.ctx, body);
        } else if (DEVP2P_DISCONNECT == package_type) {
            err = self->settings->on_disconnect(self->base.ctx, body);
        } else if (DEVP2P_PING == package_type) {
            err = self->settings->on_ping(self->base.ctx, body);
        } else if (DEVP2P_PONG == package_type) {
            err = self->settings->on_pong(self->base.ctx, body);
        }
    }

    return err;
}

int
rlpx_devp2p_protocol_write(
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
rlpx_devp2p_protocol_write_hello(
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

    // Create body list
    urlp_push(body, urlp_item_u32(RLPX_VERSION_P2P));
    urlp_push(body, urlp_item_str(RLPX_CLIENT_ID_STR));
    urlp_push(body, caps);
    urlp_push(body, urlp_item_u32(port));
    urlp_push(body, urlp_item_u8_arr(id, 64));

    // Encode
    err = rlpx_devp2p_protocol_write(x, DEVP2P_HELLO, body, out, l);
    urlp_free(&body);
    return err;
}

int
rlpx_devp2p_protocol_write_disconnect(
    rlpx_coder* x,
    RLPX_DEVP2P_DISCONNECT_REASON reason,
    uint8_t* out,
    uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    urlp_push(rlp, urlp_item_u32((uint32_t)reason));
    err = rlpx_devp2p_protocol_write(x, DEVP2P_DISCONNECT, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_devp2p_protocol_write_ping(rlpx_coder* x, uint8_t* out, uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    err = rlpx_devp2p_protocol_write(x, DEVP2P_PING, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_devp2p_protocol_write_pong(rlpx_coder* x, uint8_t* out, uint32_t* l)
{
    int err;
    urlp* rlp = urlp_list();
    if (!rlp) return -1;
    err = rlpx_devp2p_protocol_write(x, DEVP2P_PONG, rlp, out, l);
    urlp_free(&rlp);
    return err;
}

//
//
//
