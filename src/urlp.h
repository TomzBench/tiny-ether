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
 * Doubly linked list with tail pointers and extra padding for node rlp.
 *
 * ("cat")->
 * "cat"
 *
 * (0)->("cat")->
 * ["cat"]
 *
 * (0)->("dog")->("cat")->
 * ["cat","dog"]
 *
 * ...
 */
typedef struct urlp {
    struct urlp *x, *y;
    uint32_t sz;
    uint8_t b[];
} urlp;

urlp* urlp_item(const uint8_t*, uint32_t);
urlp* urlp_list(int n, ...);
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
