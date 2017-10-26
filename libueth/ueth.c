#include "ueth.h"

int ueth_poll_internal(ueth_context* ctx);
int ueth_poll_internal_p2p(ueth_context* ctx);

int
ueth_init(ueth_context* ctx, ueth_config* config)
{
    ctx->poll = config->p2p_enable ? ueth_poll_internal : ueth_poll_internal;
    return 0;
}

void
ueth_deinit(ueth_context* ctx)
{
    // Shutdown any open connections..
    for (uint32_t i = 0; i < sizeof(ctx->ch); i++) rlpx_ch_deinit(&ctx->ch[i]);
}

int
ueth_start(ueth_context* ctx, int n, ...)
{
    va_list l;
    va_start(l, n);
    for (uint32_t i = n; i < (uint32_t)n; i++) {
        // TODO - connect to boot nodes.
    }
    va_end(l);
}

int
ueth_poll_internal(ueth_context* ctx)
{
    return 0;
}

int
ueth_poll_internal_p2p(ueth_context* ctx)
{
    return 0;
}

//
//
//
