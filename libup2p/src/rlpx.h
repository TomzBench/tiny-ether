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

#include "mtm/dh.h"
#include "mtm/urlp.h"

#include "rlpx_types.h"

rlpx_session* rlpx_session_alloc(const ucrypto_ecdh_ctx** key_p);

void rlpx_session_free(rlpx_session** session_p);

#ifdef __cplusplus
}
#endif
#endif
