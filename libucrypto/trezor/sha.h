#ifndef SHA_H_
#define SHA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /*!< sha3 from trezor needs size_t */

#include "sha3.h" /*!< trezor-crypto */

typedef uint8_t h256[sha3_256_hash_size];

#define sha(src, slen, dst) sha3_256(src, slen, dst)

#ifdef __cplusplus
}
#endif
#endif
