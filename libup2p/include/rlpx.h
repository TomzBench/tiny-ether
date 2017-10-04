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

#include "rlpx_config.h"
#include "rlpx_types.h"

/**
 * @brief
 *
 * rlpx_alloc()      // new session and generate new keys
 * rlpx_alloc_key()  // new session with static key
 * rlpx_alloc_keys() // new session with static and epheremeral key
 *
 * @param char

 * @return
 */

// constructors
extern rlpx_channel* rlpx_ch_alloc();
extern rlpx_channel* rlpx_ch_alloc_key(uecc_private_key*);
extern rlpx_channel* rlpx_ch_alloc_keypair(uecc_private_key*,
                                           uecc_private_key*);
extern void rlpx_ch_free(rlpx_channel** session_p);

// setters / getters
extern uint64_t rlpx_ch_version_remote(rlpx_channel*);
extern const uecc_public_key* rlpx_ch_pub_skey(rlpx_channel*);
extern const uecc_public_key* rlpx_ch_pub_ekey(rlpx_channel*);
extern const uecc_public_key* rlpx_ch_remote_pub_ekey(rlpx_channel*);
extern const uecc_public_key* rlpx_ch_remote_pub_skey(rlpx_channel*);

// methods rlpx_handshake exports
extern int rlpx_auth_read(rlpx_channel* s, const uint8_t* auth, size_t l);
extern int rlpx_auth_write(rlpx_channel* s,
                           const uecc_public_key*,
                           uint8_t* auth,
                           size_t* l);
extern int rlpx_ack_read(rlpx_channel* s, const uint8_t* auth, size_t l);
extern int rlpx_ack_write(rlpx_channel* s,
                          const uecc_public_key*,
                          uint8_t* auth,
                          size_t* l);

#ifdef __cplusplus
}
#endif
#endif
