/*
 * @file rlpx_internal.h
 *
 * @brief
 */
#ifndef RLPX_CHANNEL_H_
#define RLPX_CHANNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"

typedef struct
{
    // board_socket_fd conn;        /*!< os socket handle */
    uecc_ctx ekey;               /*!< our epheremal key */
    uecc_ctx skey;               /*!< our static key */
    h256 nonce;                  /*!< local nonce */
    char node_id[65];            /*!< node id */
    uint32_t listen_port;        /*!< our listen port */
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
} rlpx_channel;

// constructors
rlpx_channel* rlpx_ch_alloc();
rlpx_channel* rlpx_ch_alloc_key(uecc_private_key*);
rlpx_channel* rlpx_ch_alloc_keypair(uecc_private_key*, uecc_private_key*);
void rlpx_ch_free(rlpx_channel** session_p);

// setters / getters
uint64_t rlpx_ch_version_remote(rlpx_channel*);
const uecc_public_key* rlpx_ch_pub_skey(rlpx_channel*);
const uecc_public_key* rlpx_ch_pub_ekey(rlpx_channel*);
const uecc_public_key* rlpx_ch_remote_pub_ekey(rlpx_channel*);
const uecc_public_key* rlpx_ch_remote_pub_skey(rlpx_channel*);
uint32_t rlpx_ch_listen_port(rlpx_channel* s);
const char* rlpx_ch_node_id(rlpx_channel* s);

int rlpx_ch_auth_load(rlpx_channel* s, const uint8_t* auth, size_t l);
int rlpx_ch_ack_load(rlpx_channel* s, const uint8_t* ack, size_t l);
#ifdef __cplusplus
}
#endif
#endif
