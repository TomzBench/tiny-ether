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
        uecc_key_init_binary(&ctx->id, &key);
    } else {
        uecc_key_init_new(&ctx->id);
    }

    // init constants
    ctx->n = (sizeof(ctx->ch) / sizeof(rlpx_io));

    // Init peer pipes (tcp)
    for (uint32_t i = 0; i < ctx->n; i++) {
        rlpx_io_init_tcp(&ctx->ch[i], &ctx->id, &ctx->config.udp, NULL);
        rlpx_io_devp2p_install(&ctx->ch[i]);
    }

    // Init discovery pipe
    // rlpx_io_init_udp(&ctx->discovery, &ctx->id, &ctx->config.udp, NULL);
    // rlpx_io_discovery_install(&ctx->discovery);

    return 0;
}

void
ueth_deinit(ueth_context* ctx)
{
    // Shutdown any open connections..
    for (uint32_t i = 0; i < ctx->n; i++) rlpx_io_deinit(&ctx->ch[i]);

    // Shutdown udp
    // rlpx_io_deinit(&ctx->discovery);

    // Free static key
    uecc_key_deinit(&ctx->id);
}

int
ueth_start(ueth_context* ctx, int n, ...)
{
    va_list l;
    va_start(l, n);
    const char* enode;
    for (uint32_t i = 0; i < (uint32_t)n; i++) {
        enode = va_arg(l, const char*);
        rlpx_io_connect_enode(&ctx->ch[i], enode);
    }
    va_end(l);
    return 0;
}

int
ueth_stop(ueth_context* ctx)
{
    uint32_t mask = 0, i, c = 0, b = 0;
    rlpx_io* ch[ctx->n];
    rlpx_io_devp2p* devp2p;
    for (i = 0; i < ctx->n; i++) {
        if (rlpx_io_is_ready(&ctx->ch[i])) {
            mask |= (1 << i);
            ch[b++] = &ctx->ch[i];
            devp2p = ctx->ch[i].protocols[0].context;
            rlpx_io_devp2p_send_disconnect(devp2p, DEVP2P_DISCONNECT_QUITTING);
        }
    }
    while (mask && ++c < 50) {
        usys_msleep(100);
        rlpx_io_poll(ch, b, 100);
        for (i = 0; i < ctx->n; i++) {
            if (rlpx_io_is_shutdown(devp2p->base)) mask &= (~(1 << i));
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
        if (ctx->ch[i].node.port_tcp) {
            ch[b++] = &ctx->ch[i];
            // If this channel is not connected
            if (!rlpx_io_is_connected(&ctx->ch[i])) {
                rlpx_io_nonce(&ctx->ch[i]);
                rlpx_io_connect_node(&ctx->ch[i], &ctx->ch[i].node);
            }
        }
    }

    // Add our listener to poll
    // TODO - io driver shuts down socket on error, need to fix that.
    ////if (ctx->discovery.io.sock < 0) rlpx_io_listen(&ctx->discovery);
    ////ch[b++] = &ctx->discovery;
    rlpx_io_poll(ch, b, 100);
    return 0;
}

//
//
//
