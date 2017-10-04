/*
 * @file rlpx_handshake.h
 *
 * @brief
 */

#ifndef RLPX_HANDSHAKE_H_
#define RLPX_HANDSHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_channel.h"

#define RLPX_MIN_PAD 100
#define RLPX_MAX_PAD 250

int rlpx_auth_read(rlpx_channel* s, const uint8_t* auth, size_t l);
int rlpx_auth_write(uecc_ctx* skey,
                    uecc_ctx* ekey,
                    h256* nonce,
                    const uecc_public_key* to_s_key,
                    uint8_t* auth_p,
                    size_t* l);
int rlpx_ack_read(rlpx_channel* s, const uint8_t* auth, size_t l);
int rlpx_ack_write(uecc_ctx* skey,
                   uecc_ctx* ekey,
                   h256* nonce,
                   const uecc_public_key* to_s_key,
                   uint8_t* auth_p,
                   size_t* l);

int rlpx_secrets(rlpx_channel* s,
                 int orig,
                 uint8_t* sent,
                 uint32_t sentlen,
                 uint8_t* recv,
                 uint32_t recvlen);

#ifdef __cplusplus
}
#endif
#endif
