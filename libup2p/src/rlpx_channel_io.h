#ifndef RLPX_CHANNEL_IO_H_
#define RLPX_CHANNEL_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"

int rlpx_ch_io_on_recv(void* ctx, uint8_t* b, uint32_t l);
int rlpx_ch_io_on_recv_ack(void* ctx, uint8_t* b, uint32_t l);
int rlpx_ch_io_on_recv_auth(void* ctx, uint8_t* b, uint32_t l);

#ifdef __cplusplus
}
#endif
#endif
