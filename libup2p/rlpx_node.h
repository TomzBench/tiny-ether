/**
 * @file rlpx_node.h
 *
 * @brief Endpoint context for storing methods and metadata for a remote
 * endpoint.
 */
#ifndef RLPX_NODE_H_
#define RLPX_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uecc.h"

typedef struct
{
    uecc_public_key id;
    char ip_v4[16];
    uint32_t tcp_port;
    uint32_t udp_port;
} rlpx_node;

int rlpx_node_init(rlpx_node* self,
                   uecc_public_key* id,
                   const char* host,
                   uint32_t tcp,
                   uint32_t udp);
int rlpx_node_init_enode(rlpx_node* self, const char* enode);
int rlpx_node_deinit(rlpx_node* self);

#ifdef __cplusplus
}
#endif
#endif
