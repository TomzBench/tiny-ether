/*
 * @file rlpx_internal.h
 *
 * @brief
 */
#ifndef RLPX_INTERNAL_H_
#define RLPX_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "rlpx_frame_hello.h"
#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"

#define AES_LEN(l) ((l) % 16 ? ((l) + 16 - ((l) % 16)) : (l))

#define READ_BE(l, dst, src)                                                   \
    do {                                                                       \
        uint32_t be = 1;                                                       \
        if (*(char*)&be == 0) {                                                \
            for (int i = 0; i < l; i++) ((uint8_t*)dst)[i] = src[i];           \
        } else {                                                               \
            for (int i = 0; i < l; i++) ((uint8_t*)dst)[(l)-1 - i] = src[i];   \
        }                                                                      \
    } while (0)

#define XORN(x, b, l)                                                          \
    do {                                                                       \
        for (int i = 0; i < l; i++) x[i] ^= b[i];                              \
    } while (0)

#define XOR32(x, b)                                                            \
    do {                                                                       \
        for (int i = 0; i < 32; i++) x[i] ^= b[i];                             \
    } while (0)

#define XOR32_SET(y, x, b)                                                     \
    do {                                                                       \
        for (int i = 0; i < 32; i++) y[i] = x[i] ^ b[i];                       \
    } while (0)

typedef struct
{
    // board_socket_fd conn;        /*!< os socket handle */
    uecc_ctx ekey;               /*!< our epheremal key */
    uecc_ctx skey;               /*!< our static key */
    h256 nonce;                  /*!< local nonce */
    uint64_t remote_version;     /*!< remote version from auth */
    h512 remote_node_id;         /*!< remote public address */
    h256 remote_nonce;           /*!< remote nonce */
    uecc_public_key remote_ekey; /*!< remote ephermeral pubkey */
    uecc_public_key remote_skey; /*!< remote static pubkey */
    ukeccak256_ctx emac;         /*!< egress mac */
    ukeccak256_ctx imac;         /*!< ingress mac */
    uaes_ctx aes_dec;            /*!< aes dec */
    uaes_ctx aes_enc;            /*!< aes dec */
    uaes_ctx aes_mac;            /*!< aes ecb of egress/ingress mac updates */
} rlpx;

// constructors
rlpx* rlpx_alloc();
rlpx* rlpx_alloc_key(uecc_private_key*);
rlpx* rlpx_alloc_keypair(uecc_private_key*, uecc_private_key*);
void rlpx_free(rlpx** session_p);

// setters / getters
uint64_t rlpx_version_remote(rlpx*);
const uecc_public_key* rlpx_public_skey(rlpx*);
const uecc_public_key* rlpx_public_ekey(rlpx*);
const uecc_public_key* rlpx_remote_public_ekey(rlpx*);
const uecc_public_key* rlpx_remote_public_skey(rlpx*);

#ifdef __cplusplus
}
#endif
#endif
