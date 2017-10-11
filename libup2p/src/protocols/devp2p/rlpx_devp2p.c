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
    rlpx_devp2p_protocol* self = (rlpx_devp2p_protocol*)base;
    RLPX_DEVP2P_PROTOCOL_PACKET_TYPE type = rlpx_frame_type(rlp);
    rlp = rlpx_frame_body(rlp);
    if (DEVP2P_HELLO == type) {
        err = self->settings->on_hello(self->base.ctx, rlp);
    } else if (DEVP2P_DISCONNECT == type) {
        err = self->settings->on_disconnect(self->base.ctx, rlp);
    } else if (DEVP2P_PING == type) {
        err = self->settings->on_ping(self->base.ctx, rlp);
    } else if (DEVP2P_PONG == type) {
        err = self->settings->on_pong(self->base.ctx, rlp);
    }

    return err;
}
