#include "ktable.h"
#include "urand.h"

int
ktable_init(ktable* ctx, knode* id)
{
    ctx->node = *id;
    return 0;
}

void
ktable_deinit(ktable* ctx)
{
    ((void)ctx);
}

int
ktable_add_node(ktable* ctx, knode* node)
{
    return 0;
}

uint32_t
ktable_buckets_len(ktable* ctx)
{
    return 0;
}
