#ifndef UETH_H_
#define UETH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ueth_config.h"

#include "rlpx_channel.h"

typedef struct
{
    rlpx_channel ch[UETH_CONFIG_NUM_CHANNELS];
} ueth_context;

#ifdef __cplusplus
}
#endif
#endif
