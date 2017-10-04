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
void rlpx_test_remote_nonce_set(rlpx_channel* s, h256* nonce);
void rlpx_test_nonce_set(rlpx_channel* s, h256* nonce);
void rlpx_test_remote_ekey_clr(rlpx_channel* s);
ukeccak256_ctx* rlpx_test_ingress(rlpx_channel* ch);
ukeccak256_ctx* rlpx_test_egress(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_mac(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_enc(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_dec(rlpx_channel* ch);
uint32_t rlpx_ch_listen_port(rlpx_channel* s);
const char* rlpx_ch_node_id(rlpx_channel* s);
int rlpx_test_secrets(rlpx_channel* s,
                      int orig,
                      uint8_t* sent,
                      uint32_t sentlen,
                      uint8_t* recv,
                      uint32_t recvlen);
int rlpx_expect_secrets(rlpx_channel* s,
                        int orig,
                        uint8_t* sent,
                        uint32_t sentlen,
                        uint8_t* recv,
                        uint32_t recvlen,
                        uint8_t* aes,
                        uint8_t* mac,
                        uint8_t* foo);
int rlpx_secrets(rlpx_channel* s,
                 int orig,
                 uint8_t* sent,
                 uint32_t sentlen,
                 uint8_t* recv,
                 uint32_t recvlen);

/*!< private export for test */
int rlpx_frame_parse(ukeccak256_ctx* h,
                     uaes_ctx* aes_mac,
                     uaes_ctx* aes_dec,
                     const uint8_t* frame,
                     size_t l,
                     urlp**);
int rlpx_frame_hello_write(rlpx_channel* s, uint8_t* out, size_t* l);
int rlpx_frame_hello_p2p_version(const urlp* rlp, uint32_t* out);
int rlpx_frame_hello_client_id(const urlp* rlp, const char**, uint32_t* l);
int rlpx_frame_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v);
int rlpx_frame_hello_listen_port(const urlp* rlp, uint32_t*);
int rlpx_frame_hello_node_id(const urlp* rlp, const char**, uint32_t*);

#ifdef __cplusplus
}
#endif
#endif
