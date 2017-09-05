#ifndef DH_H_
#define DH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"

/**
 * @brief wrapper object for portability.
 */
typedef mbedtls_ecdh_context crypto_ecdh_ctx;

/**
 * @brief Initialize ecdh key context onto heap
 *
 * @return Heap pointer or NULL;
 */
crypto_ecdh_ctx* crypto_ecdh_key_alloc();

/**
 * @brief initialize a key context
 *
 * @param crypto_ecdh_ctx
 *
 * @return
 */
int crypto_ecdh_key_init(crypto_ecdh_ctx*);

/**
 * @brief return any heap from key
 *
 * @param
 */
void crypto_ecdh_key_free(crypto_ecdh_ctx**);

#ifdef __cplusplus
}
#endif
#endif
