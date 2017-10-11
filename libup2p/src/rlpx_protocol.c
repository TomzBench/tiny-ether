#include "rlpx_protocol.h"

void
rlpx_protocol_init(rlpx_protocol* self,
                   uint32_t type,
                   const char* name,
                   rlpx_protocol_on_recv_fn fn)
{
    self->type = type;
    self->on_recv = fn;
    snprintf(self->cap, 6, "%s", name);
}
