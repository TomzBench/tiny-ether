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

/**
 * @file urlp.h
 *
 * @brief api and types
 */
#ifndef URLP_H_
#define URLP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "urlp_config.h"

typedef struct urlp urlp; /*!< opaque class */
typedef void (*urlp_walk_fn)(const urlp*, int, void*);

#define urlp_item(b) urlp_item_str(b)  /*!< alias */
#define urlp_is_list(rlp) (!(rlp->sz)) /*!< empty node signal start of list */

urlp* urlp_alloc(uint32_t);
void urlp_free(urlp**);
urlp* urlp_list();
urlp* urlp_item_u64(const uint64_t*, uint32_t sz);
urlp* urlp_item_u32(const uint32_t*, uint32_t sz);
urlp* urlp_item_u16(const uint16_t*, uint32_t sz);
urlp* urlp_item_u8(const uint8_t*, uint32_t);
urlp* urlp_item_str(const char*);
urlp* urlp_item_mem(const uint8_t* b, uint32_t l);
int urlp_idx_to_u64(const urlp* rlp, uint32_t idx, uint64_t* val);
int urlp_idx_to_u32(const urlp* rlp, uint32_t idx, uint32_t* val);
int urlp_idx_to_u16(const urlp* rlp, uint32_t idx, uint16_t* val);
int urlp_idx_to_u8(const urlp* rlp, uint32_t idx, uint8_t* val);
int urlp_idx_to_mem(const urlp* rlp, uint32_t idx, uint8_t* mem, uint32_t* l);
int urlp_idx_to_str(const urlp* rlp, uint32_t idx, char* str);
uint64_t urlp_unsafe_idx_as_u64(const urlp* rlp, uint32_t idx);
uint32_t urlp_unsafe_idx_as_u32(const urlp* rlp, uint32_t idx);
uint16_t urlp_unsafe_idx_as_u16(const urlp* rlp, uint32_t idx);
uint8_t urlp_unsafe_idx_as_u8(const urlp* rlp, uint32_t idx);
const uint8_t* urlp_unsafe_idx_as_mem(const urlp* rlp, uint32_t idx);
const char* urlp_unsafe_idx_as_str(const urlp* rlp, uint32_t idx);
uint64_t urlp_as_u64(const urlp*);
uint32_t urlp_as_u32(const urlp*);
uint16_t urlp_as_u16(const urlp*);
uint8_t urlp_as_u8(const urlp*);
const char* urlp_as_str(const urlp* rlp);
const uint8_t* urlp_as_mem(const urlp* rlp, uint32_t*);
const uint8_t* urlp_ref(const urlp*, uint32_t*);
urlp* urlp_copy(const urlp*);
int urlp_read_int(const urlp*, void* m, uint32_t);
const urlp* urlp_at(const urlp*, uint32_t);
urlp* urlp_push(urlp*, urlp*);
uint32_t urlp_size_rlp(const urlp* rlp);
uint32_t urlp_size(const urlp*);
const uint8_t* urlp_data(urlp* rlp);
const urlp* urlp_child(const urlp* rlp);
uint32_t urlp_children(const urlp* rlp);
uint32_t urlp_children_walk(const urlp* rlp);
uint32_t urlp_siblings(const urlp* rlp);
uint32_t urlp_print_size(const urlp* rlp);
int urlp_print(const urlp* rlp, uint8_t* b, uint32_t* sz);
urlp* urlp_parse(const uint8_t* b, uint32_t);
void urlp_foreach(const urlp* rlp, void* ctx, urlp_walk_fn fn);

#ifdef __cplusplus
}
#endif
#endif
