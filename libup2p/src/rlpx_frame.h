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
 * cpp-ethereum reference, From handshake -> to setting up for
 * RLPX framing multiplex
 *
 * RLPXFrameCoder::RLPXFrameCoder(RLPXHandshake const& _init):
 * 	m_impl(new RLPXFrameCoderImpl)
 * 	{
 *		RLPXFrameCoder::setup(...
 * 	}
 *
 *Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
 *	        ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
 *Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
 *	        ingress-mac: sha3(mac-secret^recipient-nonce || auth-recvd-init)
 **/

#include "uaes.h"

#ifdef __cplusplus
}
#endif
#endif
