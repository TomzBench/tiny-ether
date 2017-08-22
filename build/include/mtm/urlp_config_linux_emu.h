#ifndef URLP_CONFIG_LINUX_EMU_H_
#define URLP_CONFIG_LINUX_EMU_H_

//#include <assert.h>
//#include <stdarg.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define URLP_CONFIG_ANYSIZE_ARRAY 1

#define urlp_malloc_fn malloc
#define urlp_clz_fn __builtin_clz

#endif
