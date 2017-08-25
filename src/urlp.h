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

/**
 * @brief urlp context
 * extra padding b[] for node rlp.
 */
typedef struct urlp {
    struct urlp *next, *child;
    uint32_t sz;
    uint8_t b[];
} urlp;

urlp* urlp_alloc(uint32_t);
urlp* urlp_item(const uint8_t*, uint32_t);
urlp* urlp_push(urlp*, urlp*);
uint32_t urlp_size(urlp* rlp);
const uint8_t* urlp_data(urlp* rlp);
uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t sz);
void urlp_free(urlp**);

// An empty node signal start of list
#define urlp_is_list(rlp) (!(rlp->sz))

#ifdef __cplusplus
}
#endif
#endif
