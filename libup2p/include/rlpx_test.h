#ifndef RLPX_TEST_H_
#define RLPX_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_types.h"
#include <stddef.h>
#include <stdint.h>

/*!< fake nonce for test */
void rlpx_test_remote_nonce_set(rlpx* s, h256* nonce);
void rlpx_test_nonce_set(rlpx* s, h256* nonce);

#ifdef __cplusplus
}
#endif
#endif
