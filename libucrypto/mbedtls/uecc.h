#ifndef DH_H_
#define DH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"

#include "umpi.h"

/*!< r: [0, 32), s: [32, 64), v: 64 */
// typedef uint8_t uecc_signature[65];
// typedef uint8_t uecc_public_key[65];
typedef h520 uecc_signature;
typedef h520 uecc_public_key;
typedef ubn uecc_private_key;
typedef mbedtls_ecdh_context uecc_ctx; /*!< caller ref */
typedef mbedtls_ecp_point uecc_point;  /*!< curve struct */
#define uecc_z_cmp(x, y) ubn_cmp(x, y)

/**
 * @brief initialize a key context
 *
 * @param uecc_ctx
 *
 * @return
 */
int uecc_key_init(uecc_ctx*, const ubn* d);
int uecc_key_init_string(uecc_ctx* ctx, int radix, const char*);
int uecc_key_init_binary(uecc_ctx*, const ubn* d);
int uecc_key_init_new(uecc_ctx*);

/**
 * @brief
 *
 * @param uecc_ctx
 */
void uecc_key_deinit(uecc_ctx*);

/**
 * @brief
 *
 * @param str
 * @param rdx
 * @param q
 *
 * @return
 */
int uecc_atop(const char* str, int rdx, uecc_point* q);

/**
 * @brief
 *
 * @param uecc_ctx
 * @param b
 *
 * @return
 */
int uecc_ptob(uecc_point*, uecc_public_key* b);

/**
 * @brief
 *
 * @param k
 * @param p
 *
 * @return
 */
int uecc_btop(uecc_public_key* k, uecc_point* p);

/**
 * @brief
 *
 * @param src
 * @param dst
 *
 * @return
 */
int uecc_point_copy(const uecc_point* src, uecc_point* dst);

/**
 * @brief
 *
 * @param src
 * @param dst
 *
 * @return
 */
int uecc_point_cmp(const uecc_point* src, const uecc_point* dst);

/**
 * @brief
 *
 * @param p
 *
 * @return
 */
int uecc_ztoa(const uecc_point* p);

/**
 * @brief
 *
 * @param ctx
 * @param k
 *
 * @return
 */
int uecc_agree(uecc_ctx* ctx, const uecc_public_key* k);

/**
 * @brief Compute a shared secret. Secret is updated into ctx z param
 *
 * @param ctx
 *
 * @return 0 OK or -1 err
 */
int uecc_agree_point(uecc_ctx* ctx, const uecc_point*);

/**
 * @brief
 *
 * @param ctx
 * @param b
 * @param sz
 * @param uecc_signature
 *
 * @return
 */
int uecc_sign(uecc_ctx* ctx, const uint8_t* b, uint32_t sz, uecc_signature*);

/**
 * @brief
 *
 * @param q
 * @param b
 * @param sz
 * @param uecc_signature
 *
 * @return
 */
int uecc_verify(const uecc_point* q,
                const uint8_t* b,
                uint32_t sz,
                uecc_signature*);

#ifdef __cplusplus
}
#endif
#endif
