#ifndef RLPX_TEST_H_
#define RLPX_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_types.h"
#include "uaes.h"
#include "ukeccak256.h"
#include <stddef.h>
#include <stdint.h>

/*!< fake nonce for test */
void rlpx_test_remote_nonce_set(rlpx* s, h256* nonce);
void rlpx_test_nonce_set(rlpx* s, h256* nonce);
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
int rlpx_frame_parse(ukeccak256_ctx* ingress,
                     uaes_ctx* mac,
                     uint8_t* frame,
                     size_t l);
int rlpx_test_hello(rlpx* s, const uint8_t* data, size_t l);

#ifdef __cplusplus
}
#endif
#endif
