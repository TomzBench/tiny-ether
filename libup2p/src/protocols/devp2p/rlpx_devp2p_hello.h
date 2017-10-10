#ifndef RLPX_DEVP2P_HELLO_H_
#define RLPX_DEVP2P_HELLO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uaes.h"
#include "uecc.h"
#include "urlp.h"

#include "ukeccak256.h"

int rlpx_devp2p_hello_write(uint32_t, const char*, uint8_t*, size_t*);
int rlpx_devp2p_hello_p2p_version(const urlp* rlp, uint32_t* out);
int rlpx_devp2p_hello_client_id(const urlp* rlp, const char**, uint32_t* l);
int rlpx_devp2p_hello_capabilities(const urlp*, const char*, uint32_t);
int rlpx_devp2p_hello_listen_port(const urlp* rlp, uint32_t*);
int rlpx_devp2p_hello_node_id(const urlp* rlp, const char**, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
