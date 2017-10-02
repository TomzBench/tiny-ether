#include "rlpx_frame_hello.h"

int
rlpx_frame_hello_p2p_version(const urlp* rlp, const char** ptr_p, size_t* l)
{
    if (!(rlp = urlp_at(rlp, 0))) return -1;
    *ptr_p = urlp_str(rlp);
    *l = strlen(*ptr_p);
    return 0;
}

int
rlpx_frame_hello_client_id(const urlp* rlp, const char** ptr_p, size_t* l)
{
    if (!(rlp = urlp_at(rlp, 1))) return -1;
    *ptr_p = urlp_str(rlp);
    *l = strlen(*ptr_p);
    return 0;
}

// int rlpx_frame_hello_capabilities(const urlp* rlp){}

int
rlpx_frame_hello_listen_port(const urlp* rlp, uint32_t* port)
{
    if (!(rlp = urlp_at(rlp, 3))) return -1;
    *port = urlp_as_u32(rlp);
    return 0;
}

int
rlpx_frame_hello_node_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    if (!(rlp = urlp_at(rlp, 4))) return -1;
    *ptr_p = urlp_str(rlp);
    *l = strlen(*ptr_p);
    return 0;
}
