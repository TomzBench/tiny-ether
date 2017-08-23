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
 */
typedef struct urlp {
    struct urlp *next, *child;
    uint32_t sz;
    uint8_t b[];
} urlp;

urlp* urlp_push(urlp*, urlp*);
urlp* urlp_item(uint8_t*, uint32_t);
urlp* urlp_list(int n, ...);

#ifdef __cplusplus
}
#endif
#endif
