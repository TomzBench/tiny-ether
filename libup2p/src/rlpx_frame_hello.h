#ifndef RLPX_FRAME_HELLO_H_
#define RLPX_FRAME_HELLO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_channel.h"
#include "urlp.h"

int rlpx_frame_hello_p2p_version(const urlp* rlp, uint32_t* out);
int rlpx_frame_hello_client_id(const urlp* rlp, const char**, uint32_t* l);
int rlpx_frame_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v);
int rlpx_frame_hello_listen_port(const urlp* rlp, uint32_t*);
int rlpx_frame_hello_node_id(const urlp* rlp, const char**, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
