/*
 * @file rlpx.c
 *
 * @brief
 */

#include "rlpx_handshake.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "unonce.h"
#include "urand.h"
#include "urlp.h"
#include "usha3.h"

// rlp -> cipher text
int rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t*, size_t* l);

// cipher text -> rlp
int rlpx_decrypt(uecc_ctx* ctx, const uint8_t* cipher, size_t l, urlp** rlp);

int
rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t* p, size_t* l)
{
    int err;

    // plain text size
    size_t rlpsz = urlp_print_size(rlp), padsz = urand_min_max_u8(100, 250);

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
rlpx_auth_read(rlpx* s, uint8_t* auth, size_t l)
{
    uint8_t buffer[65];
    urlp *rlp, *seek;
    int err = rlpx_decrypt(&s->skey, auth, l, &rlp);
    if (!err) {
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
            for (int i = 0; i < 32; i++) {
                x.b[i] = s->skey.z.b[i + 1] ^ s->remote_nonce.b[i];
            }
            err = uecc_recover_bin(urlp_ref(seek, NULL), &x, &s->remote_ekey);
            if (!err) {
                err = rlpx_secrets(s, &s->nonce, &s->remote_nonce, auth, l);
            }
        }
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_auth_write(rlpx* s,
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
rlpx_ack_read(rlpx* s, uint8_t* ack, size_t l)
{
    uint8_t buff[65];
    urlp *rlp, *seek;
    int err = rlpx_decrypt(&s->skey, ack, l, &rlp);
    if (!err) {
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
        err = rlpx_secrets(s, &s->remote_nonce, &s->nonce, ack, l);
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
rlpx_ack_write(rlpx* s,
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
rlpx_secrets(rlpx* s,
             h256* nonce,
             h256* initiator_nonce,
             uint8_t* cipher,
             uint32_t l)
{
    // shared-secret = sha3(ephemeral || sha3(nonce || initiator_nonce))
    // aes-secret = sha3(ephemeral || shared-secret)
    // mac-secret = sha3(ephermal || aes-secret)
    uint8_t buff[64];
    int err;
    if ((err = uecc_agree(&s->ekey, &s->remote_ekey))) return err;
    memcpy(buff, nonce->b, 32);                // left nonce
    memcpy(&buff[32], initiator_nonce->b, 32); // right nonce
    usha3(buff, 64, &buff[32], 32);            // h(nonces)
    memcpy(buff, &s->ekey.z.b[1], 32);         // (ephemeral || h(nonces))
    usha3(buff, 64, &buff[32], 32);            // shared-secret
    usha3(buff, 64, &buff[32], 32);            // aes-secret
    memcpy(s->aes_enc.b, &buff[32], 32);       // aes-secret save
    memcpy(s->aes_dec.b, &buff[32], 32);       // aes-secret save
    usha3(buff, 64, &buff[32], 32);            // mac-secret
    memcpy(s->aes_mac.b, &buff[32], 32);       // mac-secret save
    memset(s->ekey.z.b, 0, 33);                // zero mem
    memset(buff, 0, 64);                       // zero mem
    return err;
}

//
//
//
//
