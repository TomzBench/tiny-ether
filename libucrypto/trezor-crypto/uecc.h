#ifndef DH_H_
#define DH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "trezor-crypto/ecdsa.h"
#include "trezor-crypto/rand.h"

#include "umpi.h"

/*!< r: [0, 32), s: [32, 64), v: 64 */
// typedef uint8_t uecc_signature[65];
// typedef uint8_t uecc_public_key[65];
typedef h520 uecc_signature;
typedef h520 uecc_public_key;
typedef ubn uecc_private_key;
typedef h256 uecc_shared_secret;

// typedef struct
//{
//    bignum256 x, y, z;
//} uecp_point; /*!< curve struct */
//
typedef curve_point uecp_point;

typedef struct
{
    const ecdsa_curve* grp; /*!< export from trezor lib */
    ubn d;                  /*!< private key */
    uecp_point Q;           /*!< public key */
    uecp_point Qp;          /*!< remote public key */
    uecp_point z;           /*!< shared secret */
} uecc_ctx;

#define uecc_z_cmp(x, y) uecc_point_cmp(x, y)

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
int uecc_atop(const char* str, int rdx, uecp_point* q);

/**
 * @brief
 *
 * @param uecc_ctx
 * @param b
 *
 * @return
 */
int uecc_ptob(uecp_point*, uecc_public_key* b);

/**
 * @brief
 *
 * @param k
 * @param p
 *
 * @return
 */
int uecc_btop(uecc_public_key* k, uecp_point* p);

/**
 * @brief
 *
 * @param src
 * @param dst
 *
 * @return
 */
int uecc_point_copy(const uecp_point* src, uecp_point* dst);

int uecc_point_cmp(const uecp_point* src, const uecp_point* dst);

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
int uecc_agree_point(uecc_ctx* ctx, const uecp_point*);

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
int uecc_verify(const uecp_point* q,
                const uint8_t* b,
                uint32_t sz,
                uecc_signature*);

#ifdef __cplusplus
}
#endif
#endif
