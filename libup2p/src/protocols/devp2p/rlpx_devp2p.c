#include "rlpx_devp2p.h"

int rlpx_devp2p_protocol_parse(const urlp* rlp);

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
rlpx_devp2p_protocol_parse(const urlp* rlp)
{
    ((void)rlp);
    return -1;
}
