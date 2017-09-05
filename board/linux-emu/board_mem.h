#ifndef BOARD_MEM_H_
#define BOARD_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define board_alloc_fn malloc
#define board_free_fn free
#define board_alloc(x) board_alloc_fn(x)
#define board_free(x) board_free_fn(x)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
