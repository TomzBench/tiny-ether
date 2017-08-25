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
 */
typedef struct urlp {
    struct urlp *x, *y;
    uint32_t sz;
    uint8_t b[];
} urlp;

typedef void (*urlp_walk_fn)(urlp*, void*);

urlp* urlp_item(const uint8_t*, uint32_t);
urlp* urlp_list(int n, ...);
urlp* urlp_push(urlp*, urlp*);
urlp* urlp_pushx(urlp*, urlp*);
urlp* urlp_pushy(urlp*, urlp*);
urlp* urlp_npushx(int n, ...);
urlp* urlp_npushy(int n, ...);
uint32_t urlp_size(urlp* rlp);
const uint8_t* urlp_data(urlp* rlp);
void urlp_walk(urlp*, urlp_walk_fn, void*);
uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t sz);
void urlp_free(urlp**);

/*
 * Things that are not an rlp list (where[NULL]is rlp->sz==0)
 * [NULL]->
 * [NULL]->[dog]
 * [dog]->
 * #define urlp_is_list(rlp) \
 *     ((rlp->sz && rlp->x) || (!rlp->sz && rlp->x && rlp->x))
 */

#define urlp_is_list(rlp) ((rlp->sz && rlp->x) || (rlp->x && rlp->x))

#ifdef __cplusplus
}
#endif
#endif
