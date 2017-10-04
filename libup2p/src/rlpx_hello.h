#ifndef RLPX_FRAME_HELLO_H_
#define RLPX_FRAME_HELLO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uaes.h"
#include "uecc.h"
#include "urlp.h"

#include "ukeccak256.h"

int rlpx_hello_write(ukeccak256_ctx* h,
                     uaes_ctx* aes_mac,
                     uaes_ctx* aes_enc,
                     uint32_t port,
                     const char* id,
                     uint8_t* out,
                     size_t* l);
int rlpx_hello_p2p_version(const urlp* rlp, uint32_t* out);
int rlpx_hello_client_id(const urlp* rlp, const char**, uint32_t* l);
int rlpx_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v);
int rlpx_hello_listen_port(const urlp* rlp, uint32_t*);
int rlpx_hello_node_id(const urlp* rlp, const char**, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
