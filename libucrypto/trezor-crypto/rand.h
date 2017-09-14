#ifndef RAND_H_
#define RAND_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "trezor-crypto/rand.h"

int urand(void* os, uint8_t* b, size_t l);

#ifdef __cplusplus
}
#endif
#endif
