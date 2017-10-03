#include "rlpx_frame_hello.h"
#include "rlpx_frame.h"

int
rlpx_frame_hello_p2p_version(const urlp* rlp, const char** ptr_p, size_t* l)
{
    return rlpx_frame_to_mem(rlp, 0, ptr_p, l);
}

int
rlpx_frame_hello_client_id(const urlp* rlp, const char** ptr_p, size_t* l)
{
    return rlpx_frame_to_mem(rlp, 1, ptr_p, l);
}

int
rlpx_frame_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v)
{
    const urlp *caps = urlp_at(rlp, 3), *ver = NULL;
    const char* str;
    uint32_t n = 0, caplen = strlen(cap);

    // Loop capabilities, compare version number if found
    while ((rlpx_frame_to_str(caps, n++, &str)) == 0) {
        if (!(strlen(str) == caplen)) continue;
        if (memcmp(str, cap, caplen)) {
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
