/**
 * @file rlpx_types.h
 *
 * @brief
 */
#ifndef RLPX_TYPES_H_
#define RLPX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mtm/ecc.h"

typedef struct rlpx_session rlpx_session;
typedef struct
{
    uint64_t remote_version;
    ucrypto_ecp_point remote_ekey;
} rlpx_handshake;

#ifdef __cplusplus
}
#endif
#endif
