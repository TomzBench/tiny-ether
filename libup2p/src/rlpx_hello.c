#include "rlpx_hello.h"
#include "rlpx_config.h"
#include "rlpx_protocol.h"

int
rlpx_hello_write(uint32_t port, const char* id, uint8_t* out, size_t* l)
{
    int err = -1;
    uint32_t p2pver = RLPX_VERSION_P2P, les = 2, tmp = *l - 1;
    urlp *body = urlp_list(), *caps = urlp_list();

    // Create cababilities list (les/2)
    urlp_push(caps, urlp_push(urlp_item_str("les", 3), urlp_item_u32(&les, 1)));

    // Create body list
    urlp_push(body, urlp_item_u32(&p2pver, 1));
    urlp_push(body, urlp_item_str(RLPX_CLIENT_ID_STR, RLPX_CLIENT_ID_LEN));
    urlp_push(body, caps);
    urlp_push(body, urlp_item_u32(&port, 1));
    urlp_push(body, urlp_item_str(id, 65));

    err = urlp_print(body, &out[1], &tmp);
    if (!err) out[0] = 0; // message-id;
    *l = tmp + 1;
    urlp_free(&body);
    return err;
}

int
rlpx_hello_p2p_version(const urlp* rlp, uint32_t* out)
{
    return rlpx_rlp_to_u32(rlp, 0, out);
}

int
rlpx_hello_client_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    return rlpx_rlp_to_mem(rlp, 1, ptr_p, l);
}

int
rlpx_hello_capabilities(const urlp* rlp, const char* cap, uint32_t v)
{
    const urlp *seek, *caps = urlp_at(rlp, 2);
    uint32_t ver, sz, len = strlen(cap), n = caps ? urlp_siblings(caps) : 0;
    const char* mem;
    for (uint32_t i = 0; i < n; i++) {
        if (!(seek = urlp_at(caps, i))) continue;
        rlpx_rlp_to_mem(seek, 0, &mem, &sz);
        if ((sz == len) && (!(memcmp(mem, cap, len)))) {
            rlpx_rlp_to_u32(seek, 1, &ver);
            return (ver >= v) ? 0 : -1;
        }
    }

    return -1;
}

int
rlpx_hello_listen_port(const urlp* rlp, uint32_t* port)
{
    return rlpx_rlp_to_u32(rlp, 3, port);
}

int
rlpx_hello_node_id(const urlp* rlp, const char** ptr_p, uint32_t* l)
{
    return rlpx_rlp_to_mem(rlp, 4, ptr_p, l);
}
