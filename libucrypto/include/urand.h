#ifndef URAND_H_
#define URAND_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

int urand(uint8_t* b, size_t l);
int urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz);

#ifdef __cplusplus
}
#endif
#endif
