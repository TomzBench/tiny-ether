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

// rlp -> cipher text
int rlpx_encrypt(urlp* rlp,
                 const uecc_public_key* q,
                 uint8_t* cipher,
                 size_t* l);

// cipher text -> rlp
int rlpx_decrypt(uecc_ctx* ctx, const uint8_t* cipher, size_t l, urlp** rlp);

int
rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t* p, size_t* l)
{
    int err;
    static int x = 1;
    size_t rlpsz = urlp_print_size(rlp), padsz = urand_min_max_u8(100, 250);
    uint16_t sz = uecies_encrypt_size(padsz + rlpsz) + sizeof(uint16_t);
    uint8_t plain[rlpsz + padsz], *psz = (uint8_t *)&sz;
    *(uint16_t*)p = *(uint8_t*)&x ? (psz[0] << 8 | psz[1]) : *(uint16_t*)psz;
    if (!(sz <= *l)) {
        *l = sz;
        return -1;
    }
    *l = sz;
    if (!(urlp_print(rlp, plain, rlpsz) == rlpsz)) return -1;
    urand(&plain[rlpsz], padsz);
    err = uecies_encrypt(q, p, 2, plain, padsz + rlpsz, &p[2]);
    return err;
}

int
rlpx_decrypt(uecc_ctx* ecc, const uint8_t* c, size_t l, urlp** rlp_p)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (c[0] << 8 | c[1]) : *(uint16_t*)c;
    uint8_t buffer[sz];
    int err = uecies_decrypt(ecc, c, 2, &c[2], l - 2, buffer);
    return ((err > 0) && (*rlp_p = urlp_parse(buffer, err))) ? 0 : -1;
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
rlpx_write_ack(rlpx* s,
               const uecc_public_key* from_e_key,
               const uecc_public_key* to_s_key,
               uint8_t* auth,
               size_t* l)
{
    h520 ekey;
    h256 nonce;
    urlp* rlp;
    uint64_t ver = 4;
    int err = 0;
    if (!from_e_key) from_e_key = &s->ekey.Q;
    if (!to_s_key) to_s_key = &s->remote_skey;
    if (uecc_qtob(from_e_key, ekey.b, sizeof(ekey.b))) return -1;
    if (unonce(nonce.b)) return -1;
    rlp = urlp_list();
    if (rlp) {
        urlp_push(rlp, urlp_item_u8(&ekey.b[1], 64));
        urlp_push(rlp, urlp_item_u8(nonce.b, 32));
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
rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l)
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
        }
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_read_ack(rlpx* s, uint8_t* ack, size_t l)
{
    uint8_t buffer[65];
    urlp *rlp, *seek;
    int err = rlpx_decrypt(&s->skey, ack, l, &rlp);
    if (!err) {
        if ((seek = urlp_at(rlp, 0)) &&
            (urlp_size(seek) == sizeof(uecc_public_key))) {
            buffer[0] = 0x04;
            memcpy(&buffer[1], urlp_ref(seek, NULL), urlp_size(seek));
            uecc_btoq(buffer, 65, &s->remote_ekey);
        }
        if ((seek = urlp_at(rlp, 1)) && (urlp_size(seek) == sizeof(h256))) {
            memcpy(s->remote_nonce.b, urlp_ref(seek, NULL), sizeof(h256));
        }
        if ((seek = urlp_at(rlp, 2))) s->remote_version = urlp_as_u64(seek);
        urlp_free(&rlp);
        err = 0;
    }
    return err;
}

//
//
//
//
