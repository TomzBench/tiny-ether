#include "rlpx_frame.h"
#include "rlpx_handshake.h"
#include "rlpx_internal.h"
#include "ukeccak256.h"
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

void
rlpx_test_remote_ekey_clr(rlpx* s)
{
    memset(s->remote_ekey.data, 0, 64);
}

int
rlpx_test_secrets(rlpx* s,
                  int orig,
                  uint8_t* sent,
                  uint32_t sentlen,
                  uint8_t* recv,
                  uint32_t recvlen)
{
    return rlpx_secrets(s, orig, sent, sentlen, recv, recvlen);
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
    ukeccak256(buf, 64, out, 32);        // h(nonces)
    memcpy(buf, &s->ekey.z.b[1], 32);    // (ephemeral || h(nonces))
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(nonces))
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(shared))
    uaes_init_bin(&s->aes_enc, out, 32); // aes-secret save
    uaes_init_bin(&s->aes_dec, out, 32); // aes-secret save
    if (memcmp(out, aes, 32)) return -1; // test
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(aes-secret))
    uaes_init_bin(&s->aes_mac, out, 32); // mac-secret save
    if (memcmp(out, mac, 32)) return -1; // test

    // ingress / egress
    ukeccak256_init(&s->emac);
    ukeccak256_init(&s->imac);
    XOR32_SET(buf, out, s->nonce.b); // (mac-secret^recepient-nonce);
    memcpy(&buf[32], recv, recvlen); // (m..^nonce)||auth-recv-init)
    ukeccak256_update(&s->imac, buf, 32 + recvlen); // S(m..^nonce)||auth-recv)
    XOR32(buf, s->nonce.b);                         // UNDO xor
    XOR32(buf, s->remote_nonce.b);                  // (mac-secret^nonce);
    memcpy(&buf[32], sent, sentlen); // (m..^nonce)||auth-sentd-init)
    ukeccak256_update(&s->emac, buf, 32 + sentlen); // S(m..^nonce)||auth-sent)

    // test foo
    if (foo) {
        if (orig) {
            ukeccak256_update(&s->emac, (uint8_t*)"foo", 3);
            ukeccak256_digest(&s->emac, out);
        } else {
            ukeccak256_update(&s->imac, (uint8_t*)"foo", 3);
            ukeccak256_digest(&s->imac, out);
        }
        if (memcmp(out, foo, 32)) return -1;
    }

    return err;
}

//
//
//
