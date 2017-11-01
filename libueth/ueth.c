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

#include "ueth.h"
#include "usys_log.h"
#include "usys_time.h"

int ueth_poll_internal(ueth_context* ctx);
int ueth_on_erro(void* ctx);
int ueth_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);
int ueth_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);

int
ueth_init(ueth_context* ctx, ueth_config* config)
{
    h256 key;

    // Copy config.
    ctx->config = *config;

    // Polling mode (p2p enable, etc)
    ctx->poll = ueth_poll_internal;

    if (config->p2p_private_key) {
        rlpx_node_hex_to_bin(config->p2p_private_key, 0, key.b, NULL);
        uecc_key_init_binary(&ctx->p2p_static_key, &key);
    } else {
        uecc_key_init_new(&ctx->p2p_static_key);
    }

    // init constants
    ctx->n = (sizeof(ctx->ch) / sizeof(rlpx_io));

    // Init peer pipes
    for (uint32_t i = 0; i < ctx->n; i++) {
        rlpx_io_devp2p_init(
            &ctx->ch[i],
            &ctx->p2p_static_key,
            &ctx->config.udp,
            NULL,
            &ctx->ch[i]);
    }

    //// Setup udp listener
    // rlpx_io_init_discv4(
    //    &ctx->discovery, &ctx->p2p_static_key, &ctx->config.udp);

    char hex[129];
    uint8_t q[65];
    uecc_qtob(&ctx->p2p_static_key.Q, q, 65);
    rlpx_node_bin_to_hex(&q[1], 64, hex, NULL);
    usys_log_info("enode://%s:%d", hex, ctx->config.udp);

    return 0;
}

void
ueth_deinit(ueth_context* ctx)
{
    // Shutdown any open connections..
    for (uint32_t i = 0; i < ctx->n; i++) rlpx_io_devp2p_deinit(&ctx->ch[i]);

    // Shutdown udp
    // rlpx_io_deinit(&ctx->discovery);

    // Free static key
    uecc_key_deinit(&ctx->p2p_static_key);
}

int
ueth_start(ueth_context* ctx, int n, ...)
{
    va_list l;
    va_start(l, n);
    const char* enode;
    for (uint32_t i = 0; i < (uint32_t)n; i++) {
        enode = va_arg(l, const char*);
        rlpx_io_connect_enode(&ctx->ch[i].base, enode);
    }
    va_end(l);
    return 0;
}

int
ueth_stop(ueth_context* ctx)
{
    uint32_t mask = 0, i, c = 0, b = 0;
    rlpx_io* ch[ctx->n];
    for (i = 0; i < ctx->n; i++) {
        if (rlpx_io_is_connected(&ctx->ch[i].base)) {
            mask |= (1 << i);
            ch[b++] = &ctx->ch[i].base;
            rlpx_io_send_disconnect(&ctx->ch[i], DEVP2P_DISCONNECT_QUITTING);
        }
    }
    while (mask && ++c < 50) {
        usys_msleep(100);
        rlpx_io_poll(ch, b, 100);
        for (i = 0; i < ctx->n; i++) {
            if (rlpx_io_is_shutdown(&ctx->ch[i].base)) mask &= (~(1 << i));
        }
    }
    return 0;
}

int
ueth_poll_internal(ueth_context* ctx)
{
    uint32_t i, b = 0;
    rlpx_io* ch[ctx->n + 1];
    for (i = 0; i < ctx->n; i++) {
        // If this channel has peer
        if (ctx->ch[i].base.node.port_tcp) {
            ch[b++] = &ctx->ch[i].base;
            // If this channel is not connected
            if (!rlpx_io_is_connected(&ctx->ch[i].base)) {
                rlpx_io_nonce(&ctx->ch[i].base);
                rlpx_io_connect_node(&ctx->ch[i].base, &ctx->ch[i].base.node);
            }
        }
    }
    // Add our listener to poll
    // ch[b++] = &ctx->discovery;
    rlpx_io_poll(ch, b, 100);
    return 0;
}

//
//
//
