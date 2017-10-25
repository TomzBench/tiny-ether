#ifndef RLPX_KADEMLIA_H_
#define RLPX_KADEMLIA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "knode.h"
#include "rlpx_config.h"

typedef struct
{
    knode node;
} ktable;

// Init/Deinit routines
int ktable_init(ktable* ctx, knode* id);
void ktable_deinit(ktable* ctx);

// add node to table
int ktable_add_node(ktable* ctx, knode* node);

// remove node to table
int ktable_remove_node(ktable*, knode*);

// Get number of buckets
uint32_t ktable_buckets_len(ktable* ctx);

// Split a bucket
int ktable_buckets_split();

// methods
#ifdef __cplusplus
}
#endif
#endif
