#ifndef DH_H_
#define DH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include "mpi.h"

typedef mbedtls_ecdh_context ecdh_ctx; /*!< caller ref */
typedef mbedtls_ecp_point ecp_point;   /*!< curve struct */

/**
 * @brief Initialize ecdh key context onto heap
 *
 * @return Heap pointer or NULL;
 */
ecdh_ctx* ecdh_key_alloc();

/**
 * @brief initialize a key context
 *
 * @param ecdh_ctx
 *
 * @return
 */
int ecdh_key_init(ecdh_ctx*);

/**
 * @brief
 *
 * @param ecdh_ctx
 *
 * @return
 */
const ecp_point* ecdh_pubkey(ecdh_ctx*);

/**
 * @brief
 *
 * @param ecdh_ctx
 *
 * @return
 */
const mpi* ecdh_secret(ecdh_ctx*);

/**
 * @brief Compute a shared secret. Secret is updated into ctx z param
 *
 * @param ctx
 *
 * @return 0 OK or -1 err
 */
int ecdh_agree(ecdh_ctx* ctx, const ecp_point*);

/**
 * @brief return any heap from key
 *
 * @param
 */
void ecdh_key_free(ecdh_ctx**);

/**
 * @brief
 *
 * @param ecdh_ctx
 */
void ecdh_key_deinit(ecdh_ctx*);

#ifdef __cplusplus
}
#endif
#endif
