/*
 * @file rlpx_handshake_legacy.c
 *
 * @brief
 */
#include "rlpx_handshake_legacy.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "ukeccak256.h"
#include "unonce.h"
#include "urand.h"

int
rlpx_auth_read_legacy(rlpx* s, const uint8_t* auth, size_t l)
{
    return -1;
}

int
rlpx_auth_write_legacy(rlpx* s,
                       const uecc_public_key* to_s_key,
                       uint8_t* auth_p,
                       size_t* l)
{
    return -1;
}

int
rlpx_ack_read_legacy(rlpx* s, const uint8_t* auth, size_t l)
{
    return -1;
}

int
rlpx_ack_write_legacy(rlpx* s,
                      const uecc_public_key* to_s_key,
                      uint8_t* auth_p,
                      size_t* l)
{
    return -1;
}
