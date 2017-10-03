#include "rlpx_frame_hello.h"
#include "rlpx_frame.h"

int
rlpx_frame_hello_p2p_version(const urlp* rlp, uint32_t* out)
{
    return rlpx_frame_to_u32(rlp, 0, out);
}

int
rlpx_frame_hello_client_id(const urlp* rlp, const char** ptr_p, size_t* l)
{
    return rlpx_frame_to_mem(rlp, 1, ptr_p, l);
}

int
rlpx_frame_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v)
{
    const urlp *seek, *caps = urlp_at(rlp, 2);
    uint32_t ver, sz, len = strlen(cap), n = caps ? urlp_siblings(caps) : 0;
    const char* mem;
    for (uint32_t i = 0; i < n; i++) {
        if (!(seek = urlp_at(caps, i))) continue;
        rlpx_frame_to_mem(seek, 0, &mem, (size_t*)&sz);
        if ((sz == len) && (!(memcmp(mem, cap, len)))) {
            rlpx_frame_to_u32(seek, 1, &ver);
            return (ver >= v) ? 0 : -1;
        }
    }

    return -1;
}

int
rlpx_frame_hello_listen_port(const urlp* rlp, uint32_t* port)
{
    return rlpx_frame_to_u32(rlp, 3, port);
}

int
rlpx_frame_hello_node_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    return rlpx_frame_to_mem(rlp, 4, ptr_p, (size_t*)l);
}
