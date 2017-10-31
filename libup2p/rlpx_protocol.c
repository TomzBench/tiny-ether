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

#include "rlpx_protocol.h"

int rlpx_protocol_default_recv(rlpx_protocol*, const urlp* rlp);

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
rlpx_protocol_init(
    rlpx_protocol* self,
    uint32_t type,
    const char* name,
    void* ctx)
{
    memset(self, 0, sizeof(rlpx_protocol));
    self->type = type;
    self->ctx = ctx;
    self->recv = rlpx_protocol_default_recv;
    snprintf(self->cap, 6, "%s", name);
}

void
rlpx_protocol_deinit(rlpx_protocol* self)
{
    memset(self, 0, sizeof(rlpx_protocol));
    ((void)self);
}

int
rlpx_protocol_default_recv(rlpx_protocol* self, const urlp* rlp)
{
    // Other classes expected to override.
    ((void)rlp);
    ((void)self);
    return -1;
}
