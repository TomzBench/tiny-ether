#ifndef USYS_CONFIG_UNIX_H_
#define USYS_CONFIG_UNIX_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define usys_malloc_fn malloc
#define usys_malloc(x) usys_malloc_fn(x)

#define usys_free_fn free
#define usys_free(x) usys_free_fn(x)

#endif
