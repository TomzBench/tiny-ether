#ifndef URLPX_CONFIG_LINUX_EMU_H_
#define URLPX_CONFIG_LINUX_EMU_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define rlpx_malloc_fn malloc
#define rlpx_malloc(x) rlpx_malloc_fn(x)

#define rlpx_free_fn free
#define rlpx_free(x) rlpx_free_fn(x)

#endif
