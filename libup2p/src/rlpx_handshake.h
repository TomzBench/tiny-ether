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

#include "rlpx_internal.h"

int rlpx_auth_read(rlpx* s, uint8_t* auth, size_t l);
int rlpx_auth_write(rlpx* s,
                    const uecc_public_key* to_s_key,
                    uint8_t* auth_p,
                    size_t* l);
int rlpx_ack_read(rlpx* s, uint8_t* auth, size_t l);
int rlpx_ack_write(rlpx* s,
                   const uecc_public_key* to_s_key,
                   uint8_t* auth_p,
                   size_t* l);

#ifdef __cplusplus
}
#endif
#endif
