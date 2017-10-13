#include "rlpx_devp2p.h"

int rlpx_devp2p_protocol_parse(rlpx_protocol*, const urlp* rlp);

rlpx_devp2p_protocol*
rlpx_devp2p_protocol_alloc(const rlpx_devp2p_protocol_settings* settings)
{
    rlpx_devp2p_protocol* self = rlpx_malloc(sizeof(rlpx_devp2p_protocol));
    if (self) {
        rlpx_devp2p_protocol_init(self, settings);
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
rlpx_devp2p_protocol_init(rlpx_devp2p_protocol* self,
                          const rlpx_devp2p_protocol_settings* settings)
{
    // Iniitlize base class.
    rlpx_protocol_init(&self->base, 0, "p2p", NULL);

    // Override parse method.
    self->base.parse = rlpx_devp2p_protocol_parse;

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
rlpx_devp2p_protocol_parse(rlpx_protocol* base, const urlp* rlp)
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
rlpx_devp2p_protocol_write(rlpx_coder* x,
                           RLPX_DEVP2P_PROTOCOL_PACKET_TYPE type,
                           urlp* rlp,
                           uint8_t* out,
                           size_t* outlen)
{
    uint32_t tmp = 1;
    urlp* id = urlp_item_u32((uint32_t*)&type, 1);
    if (!id) return -1;
    int err = urlp_print(id, out, &tmp);
    if (!err) {
        if (rlp) {
            tmp = *outlen - 1;
            err = urlp_print(rlp, &out[1], &tmp);
            tmp++;
        }
        if (!err) err = rlpx_frame_write(x, 0, 0, out, tmp, out, outlen);
    }
    urlp_free(&id);
    return err;
}

int
rlpx_devp2p_protocol_write_hello(rlpx_coder* x,
                                 uint32_t port,
                                 const char* id,
                                 uint8_t* out,
                                 size_t* l)
{
    int err = -1;
    uint32_t p2pver = RLPX_VERSION_P2P, les = 2;
    urlp *body = urlp_list(), *caps = urlp_list();

    // Create cababilities list (les/2)
    urlp_push(caps, urlp_push(urlp_item_str("les", 3), urlp_item_u32(&les, 1)));

    // Create body list
    urlp_push(body, urlp_item_u32(&p2pver, 1));
    urlp_push(body, urlp_item_str(RLPX_CLIENT_ID_STR, RLPX_CLIENT_ID_LEN));
    urlp_push(body, caps);
    urlp_push(body, urlp_item_u32(&port, 1));
    urlp_push(body, urlp_item_str(id, 65));

    // Encode
    err = rlpx_devp2p_protocol_write(x, DEVP2P_HELLO, body, out, l);
    urlp_free(&body);
    return err;
}

int
rlpx_devp2p_protocol_write_disconnect(rlpx_coder* x,
                                      RLPX_DEVP2P_DISCONNECT_REASON reason,
                                      uint8_t* out,
                                      size_t* l)
{
    int err;
    urlp* body = urlp_item_u32((uint32_t*)&reason, 1);
    err = rlpx_devp2p_protocol_write(x, DEVP2P_DISCONNECT, body, out, l);
    urlp_free(&body);
    return err;
}

int
rlpx_devp2p_protocol_write_ping(rlpx_coder* x, uint8_t* out, size_t* l)
{
    return rlpx_devp2p_protocol_write(x, DEVP2P_PING, NULL, out, l);
}

int
rlpx_devp2p_protocol_write_pong(rlpx_coder* x, uint8_t* out, size_t* l)
{
    return rlpx_devp2p_protocol_write(x, DEVP2P_PONG, NULL, out, l);
}

//
//
//
