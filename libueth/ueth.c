#include "ueth.h"
#include "usys_log.h"
#include "usys_time.h"

int ueth_poll_tcp(ueth_context* ctx);
int ueth_poll_udp(ueth_context* ctx);
int ueth_on_accept(void* ctx);
int ueth_on_connect(void* ctx);
int ueth_on_erro(void* ctx);
int ueth_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);
int ueth_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);

async_io_settings g_ueth_io_settings = {
    .on_accept = NULL,       //
    .on_connect = NULL,      //
    .on_erro = ueth_on_erro, //
    .on_send = ueth_on_send, //
    .on_recv = ueth_on_recv, //
};

int
ueth_init(ueth_context* ctx, ueth_config* config)
{
    // Copy config.
    ctx->config = *config;

    // Setup udp listener
    if (ctx->config.udp) {
        async_io_init(&ctx->io, ctx, &g_ueth_io_settings);
        usys_listen_udp(&ctx->io.sock, ctx->config.udp);
    }

    // Polling mode (p2p enable, etc)
    ctx->poll = config->p2p_enable ? ueth_poll_udp : ueth_poll_tcp;

    // Static key (TODO read from file conf etc)
    uecc_key_init_new(&ctx->p2p_static_key);

    // init constants
    ctx->n = (sizeof(ctx->ch) / sizeof(rlpx_channel));

    // Init peer pipes
    for (uint32_t i = 0; i < ctx->n; i++) {
        rlpx_ch_init(&ctx->ch[i], &ctx->p2p_static_key, &ctx->config.udp);
    }

    return 0;
}

void
ueth_deinit(ueth_context* ctx)
{
    // Shutdown any open connections..
    for (uint32_t i = 0; i < ctx->n; i++) rlpx_ch_deinit(&ctx->ch[i]);

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
        rlpx_ch_connect_enode(&ctx->ch[i], enode);
    }
    va_end(l);
    return 0;
}

int
ueth_stop(ueth_context* ctx)
{
    uint32_t mask = 0, i, c = 0, b = 0;
    rlpx_channel* ch[ctx->n];
    for (i = 0; i < ctx->n; i++) {
        if (rlpx_ch_is_connected(&ctx->ch[i])) {
            mask |= (1 << i);
            ch[b++] = &ctx->ch[i];
            rlpx_ch_send_disconnect(&ctx->ch[i], DEVP2P_DISCONNECT_QUITTING);
        }
    }
    while (mask && ++c < 50) {
        usys_msleep(100);
        rlpx_ch_poll(ch, b, 100);
        for (i = 0; i < ctx->n; i++) {
            if (rlpx_ch_is_shutdown(&ctx->ch[i])) mask &= (~(1 << i));
        }
    }
    return 0;
}

int
ueth_poll_tcp(ueth_context* ctx)
{
    uint32_t i, b = 0;
    rlpx_channel* ch[ctx->n];
    for (i = 0; i < ctx->n; i++) {
        // If this channel has peer
        if (ctx->ch[i].node.port_tcp) {
            ch[b++] = &ctx->ch[i];
            // If this channel is not connected
            if (!rlpx_ch_is_connected(&ctx->ch[i])) {
                rlpx_ch_nonce(&ctx->ch[i]);
                rlpx_ch_connect_node(&ctx->ch[i], &ctx->ch[i].node);
            }
        }
    }
    rlpx_ch_poll(ch, b, 100);
    return 0;
}

int
ueth_poll_udp(ueth_context* ctx)
{
    async_io* io = &ctx->io;
    int err;

    // Poll tcp
    err = ueth_poll_tcp(ctx);

    // TODO - poll udp ports
    async_io_poll_n(&io, 1, 100);
    return 0;
}

int
ueth_on_erro(void* ctx)
{
    usys_log("[ IN] [UDP] error");
}

int
ueth_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
}

int
ueth_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
    usys_log("[ IN] [UDP] hit");
}

//
//
//
