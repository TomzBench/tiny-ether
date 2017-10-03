#ifndef RLPX_TEST_H_
#define RLPX_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_types.h"
#include "uaes.h"
#include "ukeccak256.h"
#include "urlp.h"
#include <stddef.h>
#include <stdint.h>

/*!< setters/getters for internal state (for test purposes) */
void rlpx_test_remote_nonce_set(rlpx* s, h256* nonce);
void rlpx_test_nonce_set(rlpx* s, h256* nonce);
void rlpx_test_remote_ekey_clr(rlpx* s);
int rlpx_test_secrets(rlpx* s,
                      int orig,
                      uint8_t* sent,
                      uint32_t sentlen,
                      uint8_t* recv,
                      uint32_t recvlen);
int rlpx_expect_secrets(rlpx* s,
                        int orig,
                        uint8_t* sent,
                        uint32_t sentlen,
                        uint8_t* recv,
                        uint32_t recvlen,
                        uint8_t* aes,
                        uint8_t* mac,
                        uint8_t* foo);

/*!< private export for test */
int rlpx_frame_parse(rlpx* s, const uint8_t* frame, size_t l, urlp**);
int rlpx_frame_hello_write(rlpx* s, uint8_t* out, size_t* l);
int rlpx_frame_hello_p2p_version(const urlp* rlp, uint32_t* out);
int rlpx_frame_hello_client_id(const urlp* rlp, const char**, size_t* l);
int rlpx_frame_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v);
int rlpx_frame_hello_listen_port(const urlp* rlp, uint32_t*);
int rlpx_frame_hello_node_id(const urlp* rlp, const char**, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
