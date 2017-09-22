#ifndef SHA3_H_
#define SHA3_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

int usha3(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen);

#ifdef __cplusplus
}
#endif
#endif
