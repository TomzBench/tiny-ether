#include "rlpx_kademlia.h"
#include "urand.h"

int
rlpx_kademlia_init(rlpx_kademlia_context* ctx, rlpx_kademlia_node* id)
{
    ctx->node = *id;
    return 0;
}

void
rlpx_kademlia_deinit(rlpx_kademlia_context* ctx)
{
    ((void)ctx);
}

void
rlpx_kademlia_random_node_id(rlpx_kademlia_node* node)
{
    urand(node->b, sizeof(node->b));
}

int
rlpx_kademlia_add(rlpx_kademlia_context* ctx, rlpx_kademlia_node* node)
{
    return 0;
}

uint32_t
rlpx_kademlia_buckets_len(rlpx_kademlia_context* ctx)
{
    return 0;
}
