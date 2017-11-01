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
void rlpx_protocol_init(
    rlpx_protocol* proto,
    uint32_t type,
    const char* cap,
    void* ctx);
void rlpx_protocol_deinit(rlpx_protocol*);

#ifdef __cplusplus
}
#endif
#endif
