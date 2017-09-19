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

#define urlp_item(b, l) urlp_item_str(b, l) /*!< alias */
#define urlp_is_list(rlp) (!(rlp->sz)) /*!< empty node signal start of list */

urlp* urlp_alloc(uint32_t);
void urlp_free(urlp**);
urlp* urlp_list();
urlp* urlp_item_u64(const uint64_t*, uint32_t sz);
urlp* urlp_item_u32(const uint32_t*, uint32_t sz);
urlp* urlp_item_u16(const uint16_t*, uint32_t sz);
urlp* urlp_item_u8(const uint8_t*, uint32_t);
urlp* urlp_item_str(const char*, uint32_t);
uint64_t urlp_as_u64(urlp*);
uint32_t urlp_as_u32(urlp*);
uint16_t urlp_as_u16(urlp*);
uint8_t urlp_as_u8(urlp*);
const char* urlp_str(urlp* rlp);
const uint8_t* urlp_mem(urlp* rlp, uint32_t*);
const uint8_t* urlp_ref(urlp*, uint32_t*);
int urlp_read_int(urlp*, void* m, uint32_t);
urlp* urlp_at(urlp*, uint32_t);
urlp* urlp_push(urlp*, urlp*);
uint32_t urlp_size_rlp(urlp* rlp);
uint32_t urlp_size(urlp*);
const uint8_t* urlp_data(urlp* rlp);
uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t sz);
urlp* urlp_parse(uint8_t* b, uint32_t);

#ifdef __cplusplus
}
#endif
#endif
