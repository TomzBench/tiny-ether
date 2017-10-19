/*
 * @file rlpx_handshake.h
 *
 * @brief
 */

#ifndef RLPX_HANDSHAKE_H_
#define RLPX_HANDSHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"
#include "urlp.h"

#define RLPX_MIN_PAD 100
#define RLPX_MAX_PAD 250

typedef struct
{
    uecc_ctx* ekey;
    uecc_ctx* skey;
    h256* nonce;
    h256 nonce_remote;
    uecc_public_key ekey_remote;
    uecc_public_key skey_remote;
    uint64_t version_remote;
    size_t cipher_len;
    size_t cipher_remote_len;
    uint8_t cipher[800];        /*!< cipher buffers for exchange */
    uint8_t cipher_remote[800]; /*!< cipher buffers for exchange */
} rlpx_handshake;

// Constructors
rlpx_handshake* rlpx_handshake_alloc(int orig,
                                     uecc_ctx* skey,
                                     uecc_ctx* ekey,
                                     h256* nonce,
                                     const uecc_public_key* to);
void rlpx_handshake_free(rlpx_handshake** hs_p);

/**
 * @brief - extract secrets from the handshake cipher texts and nonces
 *
 * ingress / egress
 * Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
 *           ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
 * Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
 *           ingress-mac: sha3(mac-secret^recipient-nonce || auth-recv-init)
 * egress  = sha3(mac-secret^their nonce || cipher sent )
 * ingress = sha3(mac-secret^our nonce   || cipher received)
 *
 * @param hs
 * @param x
 * @param orig
 *
 * @return
 */

int rlpx_handshake_secrets(rlpx_handshake* hs,
                           int orig,
                           ukeccak256_ctx* emac,
                           ukeccak256_ctx* imac,
                           uaes_ctx* aes_enc,
                           uaes_ctx* aes_dec,
                           uaes_ctx* aes_mac);
int rlpx_handshake_auth_init(rlpx_handshake*, const uecc_public_key*);
int rlpx_handshake_auth_install(rlpx_handshake* hs, urlp** rlp_p);
int rlpx_handshake_auth_recv(rlpx_handshake* hs,
                             const uint8_t* b,
                             size_t l,
                             urlp** rlp_p);
// Ack
int rlpx_handshake_ack_init(rlpx_handshake*, const uecc_public_key*);
int rlpx_handshake_ack_install(rlpx_handshake* hs, urlp** rlp_p);
int rlpx_handshake_ack_recv(rlpx_handshake* hs,
                            const uint8_t* b,
                            size_t l,
                            urlp** rlp_p);

#ifdef __cplusplus
}
#endif
#endif