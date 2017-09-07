#ifndef RLPX_HANDSHAKE_H_
#define RLPX_HANDSHAKE_H_

#ifdef __cplusplus
extern "C" {
#endif

// E(remote-pubk, S(ecdhe-random, ecdh-shared-secret^nonce) ||
// H(ecdhe-random-pubk) || pubk || nonce || 0x0)

//
// Write Auth message to socket and transitions to AckAuth.
// void writeAuth();
//
// Reads Auth message from socket and transitions to AckAuth.
//	void readAuth();
//
// Continues reading Auth message in EIP-8 format and transitions to
// AckAuthEIP8.
//	void readAuthEIP8();
//
// Derives ephemeral secret from signature and sets members after Auth has
// been decrypted.
// void setAuthValues(Signature const& sig,
//                   Public const& remotePubk,
//                   h256 const& remoteNonce,
//                   uint64_t remoteVersion);
//
// Write Ack message to socket and transitions to WriteHello.
//	void writeAck();
//
// Write Ack message in EIP-8 format to socket and transitions to WriteHello.
//	void writeAckEIP8();
//
// Reads Auth message from socket and transitions to WriteHello.
//	void readAck();
//
// Continues reading Ack message in EIP-8 format and transitions to
// WriteHello.
// void readAckEIP8();

#ifdef __cplusplus
}
#endif
#endif
