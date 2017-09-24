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
#include "uecc.h"

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
    h256 emac;                   /*!< egress mac */
    h256 imac;                   /*!< ingress mac */
    h256 aes_enc;                /*!< aes ctr encryption key */
    h256 aes_dec;                /*!< aes ctr decryption key */
    h256 aes_mac;                /*!< aes ecb of egress/ingress mac updates */
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
