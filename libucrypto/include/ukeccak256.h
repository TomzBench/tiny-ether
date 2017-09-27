#ifndef UKECCAK256_H_
#define UKECCAK256_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

int ukeccak256(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen);

#ifdef __cplusplus
}
#endif
#endif
