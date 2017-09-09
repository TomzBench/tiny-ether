/**
 * @file rlpx.h
 *
 * @brief
 */
#ifndef RLPX_H_
#define RLPX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mtm/ecc.h"
#include "mtm/urlp.h"

#include "rlpx_types.h"

rlpx_session* rlpx_session_alloc();

void rlpx_session_free(rlpx_session** session_p);

int rlpx_session_read_auth(rlpx_session* session,
                           const ucrypto_ecc_ctx* secret,
                           uint8_t* cipher,
                           uint32_t cipher_sz);

#ifdef __cplusplus
}
#endif
#endif
