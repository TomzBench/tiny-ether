#ifndef RLPX_KADEMLIA_H_
#define RLPX_KADEMLIA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"

typedef struct
{
    uint8_t b[32];
} rlpx_kademlia_node;

typedef struct
{
    rlpx_kademlia_node node;
} rlpx_kademlia_context;

// Init/Deinit routines
int rlpx_kademlia_init(rlpx_kademlia_context* ctx, rlpx_kademlia_node* id);
void rlpx_kademlia_deinit(rlpx_kademlia_context* ctx);

// methods
void rlpx_kademlia_random_node_id(rlpx_kademlia_node* node);
int rlpx_kademlia_add(rlpx_kademlia_context* ctx, rlpx_kademlia_node* node);
uint32_t rlpx_kademlia_buckets_len(rlpx_kademlia_context* ctx);
#ifdef __cplusplus
}
#endif
#endif
