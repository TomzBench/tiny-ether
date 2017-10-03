#include "rlpx_frame_hello.h"
#include "rlpx_frame.h"

/**
 * @brief  Encrypt a hello packet to remote receipient
 * TODO rlp can be hard coded
 *
 * @param s
 * @param out
 * @param l
 *
 * @return 0 ok -1 err
 */
int
rlpx_frame_hello_write(rlpx* s, uint8_t* out, size_t* l)
{
    uint32_t p2pver = RLPX_VERSION_P2P, les = 2;
    urlp *rlp = urlp_list(), *body = urlp_list(), *caps = urlp_list();

    // Create cababilities (les/2)
    urlp_push(caps, urlp_push(urlp_item_str("les", 3), urlp_item_u32(&les, 1)));

    // Create body packet
    urlp_push(body, urlp_item_u32(&p2pver, 1));
    urlp_push(body, urlp_item_str(RLPX_CLIENT_ID_STR, RLPX_CLIENT_ID_LEN));
    urlp_push(body, caps);
    urlp_push(body, urlp_item_u32(&s->listen_port, 1));
    urlp_push(body, urlp_item_str(s->node_id, 65));

    // [packet-type,packet-body]
    urlp_push(rlp, urlp_item("", 0)); // packet type
    urlp_push(rlp, body);             // packet body
    return rlpx_frame_write(s, 0, 0, &rlp, out, l);
}

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
