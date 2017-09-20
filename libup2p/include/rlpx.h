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
extern rlpx* rlpx_alloc_key(const char*);
extern rlpx* rlpx_alloc_keypair(const char*, const char*);
extern void rlpx_free(rlpx** session_p);

// setters / getters
extern uint64_t rlpx_version_remote(rlpx*);

// methods
extern int rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l);

#ifdef __cplusplus
}
#endif
#endif
