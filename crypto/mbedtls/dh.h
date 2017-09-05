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

typedef mbedtls_ecdh_context ecdh_ctx; /*!< caller ref */
typedef mbedtls_ecp_point ecp_point;   /*!< curve struct */
typedef struct
{
    mpi r;
    mpi s;
} ecp_signature;

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
 * @brief
 *
 * @param sig
 */
void ecp_signature_init(ecp_signature* sig);

/**
 * @brief
 *
 * @param sig
 */
void ecp_signature_free(ecp_signature* sig);

/**
 * @brief
 *
 * @param ctx
 * @param b
 * @param sz
 * @param ecp_signature
 *
 * @return
 */
int ecdh_sign(ecdh_ctx* ctx, const uint8_t* b, uint32_t sz, ecp_signature*);

/**
 * @brief
 *
 * @param q
 * @param b
 * @param sz
 * @param ecp_signature
 *
 * @return
 */
int ecdh_verify(const ecp_point* q,
                const uint8_t* b,
                uint32_t sz,
                ecp_signature*);
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
