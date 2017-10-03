#ifndef RLPX_FRAME_H_
#define RLPX_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Single-frame packet:
 * header || header-mac || frame || frame-mac
 *
 * Multi-frame packet:
 * header || header-mac || frame-0 ||
 * [ header || header-mac || frame-n || ... || ]
 * header || header-mac || frame-last || mac
 *
 * Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
 *  	       ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
 * Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
 *	         ingress-mac: sha3(mac-secret^recipient-nonce ||auth-recvd-init)
 *
 * shared-secret = sha3(ecdhe-shared-secret || sha3(nonce || initiator-nonce))
 * aes-secret = sha3(ecdhe-shared-secret || shared-secret)
 * mac-secret = sha3(ecdhe-shared-secret || aes-secret)
 **/

#include "rlpx_internal.h"

#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"
#include "urlp.h"
#include "urlp.h"

int rlpx_frame_parse(rlpx* s, const uint8_t* frame, size_t l, urlp**);
int rlpx_frame_parse_header(rlpx* s,
                            const uint8_t* header,
                            urlp** header_urlp,
                            uint32_t* body_len);
int rlpx_frame_parse_body(rlpx* s,
                          const uint8_t* body,
                          uint32_t body_len,
                          urlp** rlp);

static inline int
rlpx_frame_to_str(const urlp* rlp, int idx, const char** str_p)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *str_p = urlp_str(rlp);
    return 0;
}

static inline int
rlpx_frame_to_mem(const urlp* rlp, int idx, const char** mem_p, size_t* l)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *mem_p = (const char*)urlp_ref(rlp, (uint32_t*)l);
    return 0;
}

static inline int
rlpx_frame_to_u8(const urlp* rlp, int idx, uint8_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u8(rlp);
    return 0;
}

static inline int
rlpx_frame_to_u16(const urlp* rlp, int idx, uint16_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u16(rlp);
    return 0;
}

static inline int
rlpx_frame_to_u32(const urlp* rlp, int idx, uint32_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u32(rlp);
    return 0;
}

static inline int
rlpx_frame_to_u64(const urlp* rlp, int idx, uint64_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u64(rlp);
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif
