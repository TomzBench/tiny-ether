#ifndef RLPX_PROTOCOL_H_
#define RLPX_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "urlp.h"

typedef int (*rlpx_protocol_on_recv_fn)(int err, urlp* rlp);

typedef struct
{
    rlpx_protocol_on_recv_fn on_recv; /*!< callback */
    uint32_t type;                    /*!< type found in the rlpx header */
    char cap[6];                      /*!< capability typically 3 letters */
} rlpx_protocol;

int rlpx_protocol_init(rlpx_protocol* proto,
                       uint32_t type,
                       const char* cap,
                       rlpx_protocol_on_recv_fn fn);

static inline int
rlpx_rlp_to_str(const urlp* rlp, int idx, const char** str_p)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *str_p = urlp_str(rlp);
    return 0;
}

static inline int
rlpx_rlp_to_mem(const urlp* rlp, int idx, const char** mem_p, uint32_t* l)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *mem_p = (const char*)urlp_ref(rlp, (uint32_t*)l);
    return 0;
}

static inline int
rlpx_rlp_to_u8(const urlp* rlp, int idx, uint8_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u8(rlp);
    return 0;
}

static inline int
rlpx_rlp_to_u16(const urlp* rlp, int idx, uint16_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u16(rlp);
    return 0;
}

static inline int
rlpx_rlp_to_u32(const urlp* rlp, int idx, uint32_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u32(rlp);
    return 0;
}

static inline int
rlpx_rlp_to_u64(const urlp* rlp, int idx, uint64_t* out)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *out = urlp_as_u64(rlp);
    return 0;
}

static inline const urlp*
rlpx_frame_header(const urlp* rlp)
{
    return urlp_at(rlp, 0);
}

static inline uint32_t
rlpx_frame_type(const urlp* rlp)
{
    rlp = urlp_at(rlp, 0);
    return rlp ? urlp_as_u32(rlp) : 0;
}

static inline const urlp*
rlpx_frame_body(const urlp* rlp)
{
    rlp = urlp_at(rlp, 1);
    return rlp ? urlp_at(rlp, 1) : rlp;
}

#ifdef __cplusplus
}
#endif
#endif
