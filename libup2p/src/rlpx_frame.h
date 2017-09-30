#ifndef RLPX_FRAME_H_
#define RLPX_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Single-frame packet:
 * header || header-mac || frame || frame-mac
 *
 * Multi-frame packet:
 * header || header-mac || frame-0 ||
 * [ header || header-mac || frame-n || ... || ]
 * header || header-mac || frame-last || mac
 *
 * Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
 *  	       ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
 * Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
 *	         ingress-mac: sha3(mac-secret^recipient-nonce ||auth-recvd-init)
 *
 * shared-secret = sha3(ecdhe-shared-secret || sha3(nonce || initiator-nonce))
 * aes-secret = sha3(ecdhe-shared-secret || shared-secret)
 * mac-secret = sha3(ecdhe-shared-secret || aes-secret)
 **/

#include "rlpx_internal.h"

#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"
#include "urlp.h"

int rlpx_frame_parse(ukeccak256_ctx* ingress,
                     uaes_ctx* aes,
                     uaes_ctx* mac,
                     const uint8_t* frame,
                     size_t l);

urlp* rlpx_frame_parse_body(ukeccak256_ctx* ingress,
                            uaes_ctx* aes,
                            uaes_ctx* mac,
                            const uint8_t* frame,
                            const uint8_t* frame_mac,
                            size_t l);

#ifdef __cplusplus
}
#endif
#endif
