#include "rlpx_frame_hello.h"

int
rlpx_frame_hello_parse(rlpx_frame_hello* hello, const urlp* rlp)
{
    if (!(hello->p2p_version = urlp_at(rlp, 0))) return -1;
    if (!(hello->client_id = urlp_at(rlp, 1))) return -1;
    if (!(hello->capabilities = urlp_at(rlp, 2))) return -1;
    if (!(hello->listen_port = urlp_at(rlp, 3))) return -1;
    if (!(hello->node_id = urlp_at(rlp, 4))) return -1;
    return 0;
}

urlp*
rlpx_frame_hello_alloc(rlpx_frame_hello* hello)
{
    ((void)hello);
    return NULL;
}
