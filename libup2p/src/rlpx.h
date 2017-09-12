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

/**
 * @brief rlpx_session class
 */
rlpx_session* rlpx_session_alloc();
void rlpx_session_free(rlpx_session** session_p);

/**
 * @brief rlpx_handshake class
 */
int rlpx_read_auth(ucrypto_ecc_ctx* skey, uint8_t* auth, size_t l);

#ifdef __cplusplus
}
#endif
#endif
