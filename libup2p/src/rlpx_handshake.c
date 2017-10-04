/*
 * @file rlpx_handshake.c
 *
 * @brief
 */

#include "rlpx_handshake.h"
#include "rlpx_handshake_legacy.h"
#include "rlpx_helper_macros.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "ukeccak256.h"
#include "unonce.h"
#include "urand.h"
#include "urlp.h"

// rlp <--> cipher text
int rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t*, size_t* l);
int rlpx_decrypt(uecc_ctx* ctx, const uint8_t* cipher, size_t l, urlp** rlp);

int
rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t* p, size_t* l)
{
    int err;

    // plain text size
    size_t rlpsz = urlp_print_size(rlp);
    size_t padsz = urand_min_max_u8(RLPX_MIN_PAD, RLPX_MAX_PAD);

    // cipher prefix big endian
    uint16_t sz = uecies_encrypt_size(padsz + rlpsz) + sizeof(uint16_t);

    // Dynamic stack buffer for plain text
    uint8_t plain[rlpsz + padsz], *psz = (uint8_t *)&sz;

    // endian test
    static int x = 1;
    *(uint16_t*)p = *(uint8_t*)&x ? (psz[0] << 8 | psz[1]) : *(uint16_t*)psz;

    // Is caller buffer big enough?
    if (!(sz <= *l)) {
        *l = sz;
        return -1;
    }

    // Inform caller size, print and encrypt rlp
    *l = sz;
    if (!(urlp_print(rlp, plain, rlpsz) == rlpsz)) return -1;
    urand(&plain[rlpsz], padsz);
    err = uecies_encrypt(q, p, 2, plain, padsz + rlpsz, &p[2]);
    return err;
}

int
rlpx_decrypt(uecc_ctx* ecc, const uint8_t* c, size_t l, urlp** rlp_p)
{
    // endian test
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (c[0] << 8 | c[1]) : *(uint16_t*)c;

    // Dynamic stack buffer for cipher text
    uint8_t buffer[sz];

    // Decrypt and parse rlp
    int err = uecies_decrypt(ecc, c, 2, &c[2], l - 2, buffer);
    return ((err > 0) && (*rlp_p = urlp_parse(buffer, err))) ? 0 : -1;
}

/**
 * @brief Read RLPXHandshake
 *
 * if((seek=urlp_at(3))) { ...  //read ver
 * if((seek=urlp_at(2))) { ...  //read nonce
 * if((seek=urlp_at(1))) { ...  //read pubkey
 * if((seek=urlp_at(0))) { ...  //read signature
 *
 * @param s
 * @param auth
 * @param l
 */
int
rlpx_auth_read(rlpx_channel* s, const uint8_t* auth, size_t l)
{
    uint8_t buffer[65];
    urlp* rlp;
    const urlp* seek;
    int err = rlpx_decrypt(&s->skey, auth, l, &rlp);
    if (err) {
        return rlpx_auth_read_legacy(s, auth, l);
    } else {
        if ((seek = urlp_at(rlp, 3))) {
            // Get version
            s->remote_version = urlp_as_u64(seek);
        }
        if ((seek = urlp_at(rlp, 2)) && urlp_size(seek) == sizeof(h256)) {
            // Read remote nonce
            memcpy(s->remote_nonce.b, urlp_ref(seek, NULL), sizeof(h256));
        }
        if ((seek = urlp_at(rlp, 1)) &&
            urlp_size(seek) == sizeof(uecc_public_key)) {
            // Get secret from remote public key
            buffer[0] = 0x04;
            memcpy(&buffer[1], urlp_ref(seek, NULL), urlp_size(seek));
            uecc_btoq(buffer, 65, &s->remote_skey);
            uecc_agree(&s->skey, &s->remote_skey);
        }
        if ((seek = urlp_at(rlp, 0)) &&
            // Get remote ephemeral public key from signature
            urlp_size(seek) == sizeof(uecc_signature)) {
            uecc_shared_secret x;
            XOR32_SET(x.b, (&s->skey.z.b[1]), s->remote_nonce.b);
            err = uecc_recover_bin(urlp_ref(seek, NULL), &x, &s->remote_ekey);
        }
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_auth_write(rlpx_channel* s,
                const uecc_public_key* to_s_key,
                uint8_t* auth,
                size_t* l)
{
    int err = 0;
    uint64_t v = 4;
    uint8_t rawsig[65];
    uint8_t rawpub[65];
    uecc_shared_secret x;
    uecc_signature sig;
    urlp* rlp;
    if (uecc_agree(&s->skey, to_s_key)) return -1;
    if (unonce(s->nonce.b)) return -1;
    for (int i = 0; i < 32; i++) {
        x.b[i] = s->skey.z.b[i + 1] ^ s->nonce.b[i];
    }
    if (uecc_sign(&s->ekey, x.b, 32, &sig)) return -1;
    uecc_sig_to_bin(&sig, rawsig);
    uecc_qtob(&s->skey.Q, rawpub, 65);
    if ((rlp = urlp_list())) {
        urlp_push(rlp, urlp_item_u8(rawsig, 65));
        urlp_push(rlp, urlp_item_u8(&rawpub[1], 64));
        urlp_push(rlp, urlp_item_u8(s->nonce.b, 32));
        urlp_push(rlp, urlp_item_u64(&v, 1));
    }
    err = rlpx_encrypt(rlp, to_s_key, auth, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_ack_read(rlpx_channel* s, const uint8_t* ack, size_t l)
{
    uint8_t buff[65];
    urlp* rlp;
    const urlp* seek;
    int err = rlpx_decrypt(&s->skey, ack, l, &rlp);
    if (err) {
        return rlpx_ack_read_legacy(s, ack, l);
    } else {
        if ((seek = urlp_at(rlp, 0)) &&
            (urlp_size(seek) == sizeof(uecc_public_key))) {
            buff[0] = 0x04;
            memcpy(&buff[1], urlp_ref(seek, NULL), urlp_size(seek));
            uecc_btoq(buff, 65, &s->remote_ekey);
        }
        if ((seek = urlp_at(rlp, 1)) && (urlp_size(seek) == sizeof(h256))) {
            memcpy(s->remote_nonce.b, urlp_ref(seek, NULL), sizeof(h256));
        }
        if ((seek = urlp_at(rlp, 2))) s->remote_version = urlp_as_u64(seek);
        urlp_free(&rlp); // De-alloc
    }
    return err;
}

/**
 * @brief
 *
 * @param s
 * @param from_e_key
 * @param to_s_key
 * @param auth
 * @param l
 *
 * @return
 */
int
rlpx_ack_write(rlpx_channel* s,
               const uecc_public_key* to_s_key,
               uint8_t* auth,
               size_t* l)
{
    h520 ekey;
    urlp* rlp;
    uint64_t ver = 4;
    int err = 0;
    if (!to_s_key) to_s_key = &s->remote_skey;
    if (uecc_qtob(&s->ekey.Q, ekey.b, sizeof(ekey.b))) return -1;
    if (unonce(s->nonce.b)) return -1;
    if (!(rlp = urlp_list())) return -1;
    if (rlp) {
        urlp_push(rlp, urlp_item_u8(&ekey.b[1], 64));
        urlp_push(rlp, urlp_item_u8(s->nonce.b, 32));
        urlp_push(rlp, urlp_item_u64(&ver, 1));
    }
    if (!(urlp_children(rlp) == 3)) {
        urlp_free(&rlp);
        return -1;
    }

    err = rlpx_encrypt(rlp, to_s_key, auth, l);
    urlp_free(&rlp);
    return err;
}

int
rlpx_secrets(rlpx_channel* s,
             int orig,
             uint8_t* sent,
             uint32_t sentlen,
             uint8_t* recv,
             uint32_t recvlen)
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
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(aes-secret))
    uaes_init_bin(&s->aes_mac, out, 32); // mac-secret save

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

    return err;
}

//
//
//
//
