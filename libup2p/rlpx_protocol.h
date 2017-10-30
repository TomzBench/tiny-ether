// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

#ifndef RLPX_PROTOCOL_H_
#define RLPX_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "rlpx_frame.h"
#include "urlp.h"

// Base protocol type
typedef int (*rlpx_protocol_cb)(void* ctx, const urlp* rlp);
typedef struct rlpx_protocol
{
    int (*recv)(struct rlpx_protocol*, const urlp*); /*!< process rlp packet */
    void* ctx;     /*!< protocol callback context */
    uint32_t type; /*!< type found in the rlpx header */
    char cap[6];   /*!< capability typically 3 letters */
} rlpx_protocol;
typedef int (*rlpx_protocol_recv_fn)(rlpx_protocol*, const urlp*);

// Constructurs
rlpx_protocol* rlpx_protocol_alloc(uint32_t type, const char* cap, void* ctx);
void rlpx_protocol_free(rlpx_protocol** self_p);
void rlpx_protocol_init(rlpx_protocol* proto,
                        uint32_t type,
                        const char* cap,
                        void* ctx);
void rlpx_protocol_deinit(rlpx_protocol*);

// parseing helpers
static inline int
rlpx_rlp_to_str(const urlp* rlp, int idx, const char** str_p)
{
    if (!(rlp = urlp_at(rlp, idx))) return -1;
    *str_p = urlp_as_str(rlp);
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

static inline int
rlpx_frame_header_type(const urlp* rlp)
{
    rlp = urlp_at(rlp, 0); // get header
    return rlp ? urlp_as_u16(rlp) : -1;
}

static inline int
rlpx_frame_header_context(const urlp* rlp)
{
    rlp = urlp_at(rlp, 0); // get header
    return (rlp && (rlp = urlp_at(rlp, 1))) ? urlp_as_u16(rlp) : -1;
}

static inline const urlp*
rlpx_frame_body(const urlp* rlp)
{
    return urlp_at(rlp, 1);
}

#ifdef __cplusplus
}
#endif
#endif
