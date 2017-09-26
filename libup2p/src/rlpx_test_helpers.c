#include "rlpx_internal.h"
#include "usha3.h"
/*
 * This "test" feature is only to export data from normally opaque structures.
 */

void
rlpx_test_nonce_set(rlpx* s, h256* nonce)
{
    memcpy(s->nonce.b, nonce->b, 32);
}

void
rlpx_test_remote_nonce_set(rlpx* s, h256* nonce)
{
    memcpy(s->remote_nonce.b, nonce->b, 32);
}

/**
 * @brief This routine only called from test, non-public no declarations.
 * Is a copy paste of rlpx_secrets() with memcmp()...
 * Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
 *           ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
 * Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
 *           ingress-mac: sha3(mac-secret^recipient-nonce || auth-recvd-init)
 * egress  = sha3(mac-secret^their nonce || cipher sent )
 * ingress = sha3(mac-secret^our nonce   || cipher received)
 *
 * // Encrypt Header
 * egress-mac.update(aes(mac-secret,egress-mac) ^ header-ciphertext).digest
 *
 * // Encrypt Frame
 * egress-mac.update(aes(mac-secret,egress-mac) ^
 *   left128(egress-mac.update(frame-ciphertext).digest))
 *
 * // Decrypt Header
 * ingress-mac.update(aes(mac-secret,ingress-mac) ^ header-ciphertext).digest
 *
 * // Decrypt Frame
 * ingres-mac.update(aes(mac-secret,ingres-mac) ^
 *   left128(ingres-mac.update(frame-ciphertext).digest))
 */

int
rlpx_expect_secrets(rlpx* s,
                    int orig,
                    uint8_t* sent,
                    uint32_t sentlen,
                    uint8_t* recv,
                    uint32_t recvlen,
                    uint8_t* aes,
                    uint8_t* mac,
                    uint8_t* foo)
{
    int err;
    uint8_t buf[32 + ((sentlen > recvlen) ? sentlen : recvlen)],
        *out = &buf[32];
    if ((err = uecc_agree(&s->ekey, &s->remote_ekey))) return err;
    memcpy(buf, orig ? s->remote_nonce.b : s->nonce.b, 32);
    memcpy(out, orig ? s->nonce.b : s->remote_nonce.b, 32);

    // aes-secret / mac-secret
    usha3(buf, 64, out, 32);             // h(nonces)
    memcpy(buf, &s->ekey.z.b[1], 32);    // (ephemeral || h(nonces))
    usha3(buf, 64, out, 32);             // S(ephemeral || H(nonces))
    usha3(buf, 64, out, 32);             // S(ephemeral || H(shared))
    uaes_init_bin(&s->aes, out, 32);     // aes-secret save
    if (memcmp(out, aes, 32)) return -1; // test
    usha3(buf, 64, out, 32);             // S(ephemeral || H(aes-secret))
    uaes_init_bin(&s->mac, out, 32);     // mac-secret save
    if (memcmp(out, mac, 32)) return -1; // test

    // ingress / egress
    XOR32_SET(buf, out, s->nonce.b);         // (mac-secret^recepient-nonce);
    memcpy(&buf[32], sent, sentlen);         // (m..^nonce)||auth-recvd-init)
    usha3(buf, 32 + sentlen, s->imac.b, 32); // S(mac-secret^nonce || cipher)

    // XOR32_SET(buf, out, s->remote_nonce.b);  // mac-secret^nonce
    // memcpy(out, sent, sentlen);              // mac-secret^nonce || cipher
    // usha3(buf, 32 + sentlen, s->emac.b, 32); // S(mac-secret^nonce || cipher)
    // XOR32(buf, s->remote_nonce.b);           // UNDO xor
    // XOR32(buf, s->nonce.b);                  // mac-secret^nonce
    // memcpy(out, recv, recvlen);              // mac-secret^nonce || cipher
    // usha3(buf, 32 + recvlen, s->imac.b, 32); // S(mac-secret^nonce || cipher)

    // foo test
    uint8_t test[32];
    memcpy(buf, s->imac.b, 32);
    memcpy(&buf[32], "foo", 3);
    usha3(buf, 32 + 3, test, 32);
    return err;
}

//
//
//
