/**
 * @file rlpx.h
 *
 * @brief
 */
#ifndef RLPX_H_
#define RLPX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uecc.h"
#include "urlp.h"

#include "rlpx_types.h"

/**
 * @brief
 *
 * rlpx_alloc()      // new session and generate new keys
 * rlpx_alloc_key()  // new session with static key
 * rlpx_alloc_keys() // new session with static and epheremeral key
 *
 * @param char
 *
 * @return
 */

// constructors
extern rlpx* rlpx_alloc();
extern rlpx* rlpx_alloc_key(uecc_private_key*);
extern rlpx* rlpx_alloc_keypair(uecc_private_key*, uecc_private_key*);
extern void rlpx_free(rlpx** session_p);

// setters / getters
extern uint64_t rlpx_version_remote(rlpx*);
extern const uecc_public_key* rlpx_public_skey(rlpx*);
extern const uecc_public_key* rlpx_public_ekey(rlpx*);
extern const uecc_public_key* rlpx_remote_public_ekey(rlpx*);
extern const uecc_public_key* rlpx_remote_public_skey(rlpx*);

// methods rlpx_handshake exports
extern int rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l);
extern int rlpx_read_ack(rlpx* s, uint8_t* auth, size_t l);
extern int rlpx_write_auth(rlpx* s,
                           const uecc_public_key*,
                           uint8_t* auth,
                           size_t* l);
extern int rlpx_write_ack(rlpx* s,
                          const uecc_public_key*,
                          uint8_t* auth,
                          size_t* l);

#ifdef __cplusplus
}
#endif
#endif
