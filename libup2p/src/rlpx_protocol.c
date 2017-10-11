#include "rlpx_protocol.h"

int rlpx_protocol_default_parse(const urlp* rlp);

rlpx_protocol*
rlpx_protocol_alloc(uint32_t type, const char* cap, void* ctx)
{
    rlpx_protocol* proto = rlpx_malloc(sizeof(rlpx_protocol));
    if (proto) rlpx_protocol_init(proto, type, cap, ctx);
    return proto;
}

void
rlpx_protocol_free(rlpx_protocol** self_p)
{
    rlpx_protocol* self = *self_p;
    *self_p = NULL;
    rlpx_free(self);
}

void
rlpx_protocol_init(rlpx_protocol* self,
                   uint32_t type,
                   const char* name,
                   void* ctx)
{
    memset(self, 0, sizeof(rlpx_protocol));
    self->type = type;
    self->ctx = ctx;
    self->parse = rlpx_protocol_default_parse;
    snprintf(self->cap, 6, "%s", name);
}

void
rlpx_protocol_deinit(rlpx_protocol* self)
{
    memset(self, 0, sizeof(rlpx_protocol));
    ((void)self);
}

int
rlpx_protocol_default_parse(const urlp* rlp)
{
    // Other classes expected to override.
    ((void)rlp);
    return -1;
}
