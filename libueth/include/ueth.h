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

#ifndef UETH_H_
#define UETH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ueth_config.h"

#include "rlpx_io.h"

typedef struct
{
    const char* p2p_private_key;
    int p2p_enable;
    uint32_t udp;
} ueth_config;

typedef struct ueth_context
{
    uecc_ctx p2p_static_key;
    ueth_config config;
    async_io io;
    int (*poll)(struct ueth_context*);
    uint32_t n;
    rlpx_io ch[UETH_CONFIG_NUM_CHANNELS];
} ueth_context;

int ueth_init(ueth_context* ctx, ueth_config* config);
void ueth_deinit(ueth_context* ctx);
int ueth_start(ueth_context* ctx, int, ...);
int ueth_stop(ueth_context* ctx);

static inline int
ueth_poll(ueth_context* ctx)
{
    return ctx->poll(ctx);
}

#ifdef __cplusplus
}
#endif
#endif
