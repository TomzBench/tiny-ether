#ifndef RLPX_TEST_H_
#define RLPX_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_channel.h"
#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"
#include "urlp.h"

/*!< setters/getters for internal state (for test purposes) */
uecc_ctx* rlpx_test_skey(rlpx_channel* ch);
uecc_ctx* rlpx_test_ekey(rlpx_channel* ch);
void rlpx_test_remote_nonce_set(rlpx_channel* s, h256* nonce);
void rlpx_test_nonce_set(rlpx_channel* s, h256* nonce);
void rlpx_test_remote_ekey_clr(rlpx_channel* s);
ukeccak256_ctx* rlpx_test_ingress(rlpx_channel* ch);
ukeccak256_ctx* rlpx_test_egress(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_mac(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_enc(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_dec(rlpx_channel* ch);
int rlpx_test_expect_secrets(rlpx_channel* s,
                             int orig,
                             uint8_t* sent,
                             uint32_t sentlen,
                             uint8_t* recv,
                             uint32_t recvlen,
                             uint8_t* aes,
                             uint8_t* mac,
                             uint8_t* foo);
#ifdef __cplusplus
}
#endif
#endif
