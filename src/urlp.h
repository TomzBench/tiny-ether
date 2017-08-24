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
    struct urlp *prev, *ptail;
    struct urlp *root, *rtail;
    uint32_t sz, spot;
    uint8_t b[];
} urlp;

typedef void (*urlp_walk_fn)(urlp*, void*);

urlp* urlp_item(uint8_t*, uint32_t);
urlp* urlp_push(urlp**, urlp**);
uint32_t urlp_size(urlp* rlp);
uint8_t* urlp_data(urlp* rlp);
void urlp_walk(urlp*, urlp_walk_fn, void*);
uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t sz);
void urlp_free(urlp**);

#define urlp_is_list(rlp) (!((rlp->ptail == rlp) && (rlp->rtail == rlp)))

#ifdef __cplusplus
}
#endif
#endif
