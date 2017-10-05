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

#include "rlpx_config.h"

/* OPAQUE CLASS */
typedef struct
{
    uint8_t opaque[SIZEOF_RLPX_CHANNEL];
} rlpx_channel;

#ifdef __cplusplus
}
#endif
#endif
