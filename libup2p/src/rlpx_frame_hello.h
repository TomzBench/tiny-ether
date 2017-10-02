#ifndef RLPX_FRAME_HELLO_H_
#define RLPX_FRAME_HELLO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_internal.h"
#include "urlp.h"

typedef struct
{
    const urlp* p2p_version;
    const urlp* client_id;
    const urlp* capabilities;
    const urlp* listen_port;
    const urlp* node_id;
} rlpx_frame_hello;

int rlpx_frame_hello_parse(rlpx_frame_hello* hello, const urlp* rlp);
urlp* rlpx_frame_hello_alloc(rlpx_frame_hello* hello);

#ifdef __cplusplus
}
#endif
#endif
