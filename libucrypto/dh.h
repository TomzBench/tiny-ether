#ifndef DH_H_
#define DH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"
#include "mpi.h"

/*!< r: [0, 32), s: [32, 64), v: 64 */
typedef uint8_t ucrypto_ecp_signature[65];
typedef uint8_t ucrypto_ecdh_public_key[65];
typedef mbedtls_ecdh_context ucrypto_ecdh_ctx; /*!< caller ref */
typedef mbedtls_ecp_point ucrypto_ecp_point;   /*!< curve struct */

/**
 * @brief initialize a key context
 *
 * @param ucrypto_ecdh_ctx
 *
 * @return
 */
int ucrypto_ecdh_key_init(ucrypto_ecdh_ctx*, const ucrypto_mpi* d);

/**
 * @brief
 *
 * @param ctx
 *
 * @return
 */
int ucrypto_ecdh_init_keypair(ucrypto_ecdh_ctx* ctx);

/**
 * @brief Given private key (d), calculate public key using group G.
 *
 * Q = d*G
 *
 * @param ucrypto_ecdh_ctx
 *
 * @return
 */
int ucrypto_ecdh_import_keypair(ucrypto_ecdh_ctx*, const ucrypto_mpi* d);

/**
 * @brief
 *
 * @param ucrypto_ecdh_ctx
 */
void ucrypto_ecdh_key_deinit(ucrypto_ecdh_ctx*);

/**
 * @brief
 *
 * @param ucrypto_ecdh_ctx
 *
 * @return
 */
const ucrypto_ecp_point* ucrypto_ecdh_pubkey(ucrypto_ecdh_ctx*);

/**
 * @brief
 *
 * @param ucrypto_ecdh_ctx
 *
 * @return
 */
const ucrypto_mpi* ucrypto_ecdh_secret(ucrypto_ecdh_ctx*);

/**
 * @brief
 *
 * @param str
 * @param rdx
 * @param q
 *
 * @return
 */
int ucrypto_ecdh_point_read_string(const char* str,
                                   int rdx,
                                   ucrypto_ecp_point* q);

/**
 * @brief
 *
 * @param ucrypto_ecdh_ctx
 * @param b
 *
 * @return
 */
int ucrypto_ecdh_pubkey_write(ucrypto_ecdh_ctx*, ucrypto_ecdh_public_key* b);

/**
 * @brief
 *
 * @param ctx
 * @param k
 *
 * @return
 */
int ucrypto_ecdh_agree(ucrypto_ecdh_ctx* ctx, const ucrypto_ecdh_public_key* k);

/**
 * @brief Compute a shared secret. Secret is updated into ctx z param
 *
 * @param ctx
 *
 * @return 0 OK or -1 err
 */
int ucrypto_ecdh_agree_point(ucrypto_ecdh_ctx* ctx, const ucrypto_ecp_point*);

/**
 * @brief
 *
 * @param ctx
 * @param b
 * @param sz
 * @param ucrypto_ecp_signature
 *
 * @return
 */
int ucrypto_ecdh_sign(ucrypto_ecdh_ctx* ctx,
                      const uint8_t* b,
                      uint32_t sz,
                      ucrypto_ecp_signature*);

/**
 * @brief
 *
 * @param q
 * @param b
 * @param sz
 * @param ucrypto_ecp_signature
 *
 * @return
 */
int ucrypto_ecdh_verify(const ucrypto_ecp_point* q,
                        const uint8_t* b,
                        uint32_t sz,
                        ucrypto_ecp_signature*);

#ifdef __cplusplus
}
#endif
#endif
