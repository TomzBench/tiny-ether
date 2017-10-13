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

#include "async_io.h"
#include "rlpx_config.h"
#include "rlpx_devp2p.h"

typedef struct
{
    async_io io;                 /*!< io context for network sys calls */
    rlpx_devp2p_protocol devp2p; /*!< hello/disconnect/ping/pong */
    rlpx_protocol* protocols[2]; /*!< protocol handlers */
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
    rlpx_coder x;                /*!< igress/ingress */
} rlpx_channel;

// constructors
rlpx_channel* rlpx_ch_alloc_keypair(uecc_private_key* skey,
                                    uecc_private_key* ekey);
void rlpx_ch_free(rlpx_channel** ch_p);
int rlpx_ch_init_keypair(rlpx_channel*, uecc_private_key*, uecc_private_key*);
void rlpx_ch_deinit(rlpx_channel* session);

// setters / getters
// uint64_t rlpx_ch_version_remote(rlpx_channel*);
// const uecc_public_key* rlpx_ch_pub_skey(rlpx_channel*);
// const uecc_public_key* rlpx_ch_pub_ekey(rlpx_channel*);
// const uecc_public_key* rlpx_ch_remote_pub_ekey(rlpx_channel*);
// const uecc_public_key* rlpx_ch_remote_pub_skey(rlpx_channel*);
// uint32_t rlpx_ch_listen_port(rlpx_channel* s);
// const char* rlpx_ch_node_id(rlpx_channel* s);

// methods
int rlpx_ch_auth_write(rlpx_channel* ch,
                       const uecc_public_key*,
                       uint8_t* auth,
                       size_t* l);
int rlpx_ch_auth_load(rlpx_channel* ch, const uint8_t* auth, size_t l);
int rlpx_ch_ack_write(rlpx_channel* ch,
                      const uecc_public_key*,
                      uint8_t* ack,
                      size_t* l);
int rlpx_ch_ack_load(rlpx_channel* ch, const uint8_t* ack, size_t l);

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
 * @param s
 * @param orig
 * @param sent
 * @param sentlen
 * @param recv
 * @param recvlen
 *
 * @return
 */
int rlpx_ch_secrets(rlpx_channel* s,
                    int orig,
                    uint8_t* sent,
                    uint32_t sentlen,
                    uint8_t* recv,
                    uint32_t recvlen);
int rlpx_ch_read(rlpx_channel* ch, const uint8_t* d, size_t l);
int rlpx_ch_write_hello(rlpx_channel* ch, uint8_t* out, size_t* l);
int rlpx_ch_write_disconnect(rlpx_channel* ch,
                             RLPX_DEVP2P_DISCONNECT_REASON,
                             uint8_t* out,
                             size_t* l);
int rlpx_ch_write_ping(rlpx_channel* ch, uint8_t* out, size_t* l);
int rlpx_ch_write_pong(rlpx_channel* ch, uint8_t* out, size_t* l);

#ifdef __cplusplus
}
#endif
#endif
