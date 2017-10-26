#ifndef UETH_H_
#define UETH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ueth_config.h"

#include "rlpx_channel.h"

typedef struct
{
    int p2p_enable;
} ueth_config;

typedef struct ueth_context
{
    uecc_ctx p2p_static_key;
    int (*poll)(struct ueth_context*);
    uint32_t n;
    rlpx_channel ch[UETH_CONFIG_NUM_CHANNELS];
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
