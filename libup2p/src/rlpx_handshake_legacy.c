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

// AUTH
// E(remote-pub,S(eph,s-shared^nonce) || H(eph-pub) || pub || nonce || 0x0)

// ACK
// E(remote-pub, remote-ephemeral || nonce || 0x0)

int
rlpx_auth_read_legacy(rlpx_channel* s, const uint8_t* auth, size_t l)
{
    int err = -1;
    uecc_shared_secret x;
    uint8_t b[194], rawpub[65] = { 0x04 };
    const h256* n;               // remote nonce pointer
    const uecc_public_key* pubk; // remote pubk pointer
    pubk = (uecc_public_key*)&b[sizeof(uecc_signature) + sizeof(h256)];
    n = (h256*)&b[sizeof(uecc_signature) + //
                  sizeof(h256) +           //
                  sizeof(uecc_public_key)  //
    ];
    if (!(l == 307)) return err;
    if (!(uecies_decrypt(&s->skey, NULL, 0, auth, l, b) == 194)) return err;
    memcpy(&s->remote_nonce.b, n->b, sizeof(h256));
    memcpy(&rawpub[1], pubk->data, 64);
    uecc_btoq(rawpub, 65, &s->remote_skey);
    uecc_agree(&s->skey, &s->remote_skey);
    XOR32_SET(x.b, (&s->skey.z.b[1]), s->remote_nonce.b);
    err = uecc_recover_bin(b, &x, &s->remote_ekey);
    if (!err) s->remote_version = 4;
    return err;
}

int
rlpx_auth_write_legacy(rlpx_channel* s,
                       const uecc_public_key* to_s_key,
                       uint8_t* auth,
                       size_t* l)
{
    ((void)s);
    ((void)to_s_key);
    if (*l < 5) {
        *l = 5;
        return -1;
    }
    *l = 5;
    memcpy(auth, "thhpt", 5);
    return -1;
}

int
rlpx_ack_read_legacy(rlpx_channel* s, const uint8_t* auth, size_t l)
{
    int err = -1;
    uint8_t b[194], rawpub[65] = { 0x04 };
    if (!(uecies_decrypt(&s->skey, NULL, 0, auth, l, b) > 0)) return err;
    memcpy(&rawpub[1], b, sizeof(uecc_public_key));
    memcpy(s->remote_nonce.b, ((h256*)&b[sizeof(uecc_public_key)])->b, 32);
    uecc_btoq(rawpub, 65, &s->remote_ekey);
    s->remote_version = 4;
    return 0;
}

int
rlpx_ack_write_legacy(rlpx_channel* s,
                      const uecc_public_key* to_s_key,
                      uint8_t* auth_p,
                      size_t* l)
{
    return rlpx_auth_write_legacy(s, to_s_key, auth_p, l);
}

//
//
//
