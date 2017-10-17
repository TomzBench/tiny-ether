/*
 * @file rlpx_handshake.c
 *
 * @brief
 *
 * AUTH - legacy
 * E(remote-pub,S(eph,s-shared^nonce) || H(eph-pub) || pub || nonce || 0x0)
 *
 * ACK - legacy
 * E(remote-pub, remote-ephemeral || nonce || 0x0)
 */

#include "rlpx_handshake.h"
#include "rlpx_helper_macros.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "urand.h"

// rlp <--> cipher text
int rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t*, size_t* l);
uint32_t rlpx_decrypt(uecc_ctx* ctx, const uint8_t*, size_t l, urlp** rlp);

int rlpx_handshake_auth_recv_legacy(rlpx_handshake* hs,
                                    const uint8_t* b,
                                    size_t l,
                                    urlp** rlp_p);
int rlpx_handshake_ack_recv_legacy(rlpx_handshake* hs,
                                   const uint8_t* b,
                                   size_t l,
                                   urlp** rlp_p);
int
rlpx_encrypt(urlp* rlp, const uecc_public_key* q, uint8_t* p, size_t* l)
{
    int err;

    // plain text size
    uint32_t rlpsz = urlp_print_size(rlp);
    size_t padsz = urand_min_max_u8(RLPX_MIN_PAD, RLPX_MAX_PAD);

    // cipher prefix big endian
    uint16_t prefix = uecies_encrypt_size(padsz + rlpsz), sz = prefix + 2;

    // Dynamic stack buffer for plain text
    uint8_t plain[rlpsz + padsz], *psz = (uint8_t *)&prefix;

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
    if (urlp_print(rlp, plain, &rlpsz)) return -1;
    urand(&plain[rlpsz], padsz);
    err = uecies_encrypt(q, p, 2, plain, padsz + rlpsz, &p[2]);
    return err;
}

uint32_t
rlpx_decrypt(uecc_ctx* ecc, const uint8_t* c, size_t l, urlp** rlp_p)
{
    // endian test
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (c[0] << 8 | c[1]) : *(uint16_t*)c;

    // Dynamic stack buffer for cipher text
    uint8_t buffer[sz];

    sz += 2;
    if (l < sz) return 0;

    // Decrypt and parse rlp returns 0 on err, cipher_len on OK
    int err = uecies_decrypt(ecc, c, 2, &c[2], sz - 2, buffer);
    return ((err > 0) && (*rlp_p = urlp_parse(buffer, err))) ? sz : 0;
}

rlpx_handshake*
rlpx_handshake_alloc(int orig,
                     uecc_ctx* skey,
                     uecc_ctx* ekey,
                     h256* nonce,
                     const uecc_public_key* to)
{
    rlpx_handshake* hs = rlpx_malloc(sizeof(rlpx_handshake));
    if (hs) {
        memset(hs, 0, sizeof(rlpx_handshake));
        hs->cipher_len = sizeof(hs->cipher);
        hs->cipher_remote_len = sizeof(hs->cipher_remote);
        hs->ekey = ekey;
        hs->skey = skey;
        hs->nonce = nonce;
        if (orig) {
            rlpx_handshake_auth_init(hs, to);
        } else {
            rlpx_handshake_ack_init(hs, to);
        }
    }
    return hs;
}

void
rlpx_handshake_free(rlpx_handshake** hs_p)
{
    rlpx_handshake* hs = *hs_p;
    *hs_p = NULL;
    memset(hs, 0, sizeof(rlpx_handshake));
    rlpx_free(hs);
}

int
rlpx_handshake_secrets(rlpx_handshake* hs,
                       int orig,
                       ukeccak256_ctx* emac,
                       ukeccak256_ctx* imac,
                       uaes_ctx* aes_enc,
                       uaes_ctx* aes_dec,
                       uaes_ctx* aes_mac)
{
    int err;
    uint8_t *sent = hs->cipher, *recv = hs->cipher_remote;
    uint32_t slen = hs->cipher_len, rlen = hs->cipher_remote_len;
    uint8_t buf[32 + ((slen > rlen) ? slen : rlen)], *out = &buf[32];
    if ((err = uecc_agree(hs->ekey, &hs->ekey_remote))) return err;
    memcpy(buf, orig ? hs->nonce_remote.b : hs->nonce->b, 32);
    memcpy(out, orig ? hs->nonce->b : hs->nonce_remote.b, 32);

    // aes-secret / mac-secret
    ukeccak256(buf, 64, out, 32);       // h(nonces)
    memcpy(buf, &hs->ekey->z.b[1], 32); // (ephemeral || h(nonces))
    ukeccak256(buf, 64, out, 32);       // S(ephemeral || H(nonces))
    ukeccak256(buf, 64, out, 32);       // S(ephemeral || H(shared))
    uaes_init_bin(aes_enc, out, 32);    // aes-secret save
    uaes_init_bin(aes_dec, out, 32);    // aes-secret save
    ukeccak256(buf, 64, out, 32);       // S(ephemeral || H(aes-secret))
    uaes_init_bin(aes_mac, out, 32);    // mac-secret save

    // Ingress / egress
    ukeccak256_init(emac);
    ukeccak256_init(imac);
    XOR32_SET(buf, out, hs->nonce->b);       // (mac-secret^recepient-nonce);
    memcpy(&buf[32], recv, rlen);            // (m..^nonce)||auth-recv-init)
    ukeccak256_update(imac, buf, 32 + rlen); // S(m..^nonce)||auth-recv)
    XOR32(buf, hs->nonce->b);                // UNDO xor
    XOR32(buf, hs->nonce_remote.b);          // (mac-secret^nonce);
    memcpy(&buf[32], sent, slen);            // (m..^nonce)||auth-sentd-init)
    ukeccak256_update(emac, buf, 32 + slen); // S(m..^nonce)||auth-sent)

    return err;
}

int
rlpx_handshake_auth_init(rlpx_handshake* hs, const uecc_public_key* to)
{

    int err = 0;
    uint64_t v = 4;
    uint8_t rawsig[65];
    uint8_t rawpub[65];
    uecc_shared_secret x;
    uecc_signature sig;
    urlp* rlp;
    if (uecc_agree(hs->skey, to)) return -1;
    for (int i = 0; i < 32; i++) {
        x.b[i] = hs->skey->z.b[i + 1] ^ hs->nonce->b[i];
    }
    if (uecc_sign(hs->ekey, x.b, 32, &sig)) return -1;
    uecc_sig_to_bin(&sig, rawsig);
    uecc_qtob(&hs->skey->Q, rawpub, 65);
    if ((rlp = urlp_list())) {
        urlp_push(rlp, urlp_item_u8(rawsig, 65));
        urlp_push(rlp, urlp_item_u8(&rawpub[1], 64));
        urlp_push(rlp, urlp_item_u8(hs->nonce->b, 32));
        urlp_push(rlp, urlp_item_u64(&v, 1));
    }
    err = rlpx_encrypt(rlp, to, hs->cipher, &hs->cipher_len);
    urlp_free(&rlp);
    return err;
}

int
rlpx_handshake_ack_init(rlpx_handshake* hs, const uecc_public_key* to)
{
    h520 rawekey;
    urlp* rlp;
    uint64_t ver = 4;
    int err = 0;
    if (uecc_qtob(&hs->ekey->Q, rawekey.b, sizeof(rawekey.b))) return -1;
    if (!(rlp = urlp_list())) return -1;
    if (rlp) {
        urlp_push(rlp, urlp_item_u8(&rawekey.b[1], 64));
        urlp_push(rlp, urlp_item_u8(hs->nonce->b, 32));
        urlp_push(rlp, urlp_item_u64(&ver, 1));
    }
    if (!(urlp_children(rlp) == 3)) {
        urlp_free(&rlp);
        return -1;
    }

    err = rlpx_encrypt(rlp, to, hs->cipher, &hs->cipher_len);
    urlp_free(&rlp);
    return err;
}

int
rlpx_handshake_auth_install(rlpx_handshake* hs, urlp** rlp_p)
{
    int err = -1;
    uint8_t buffer[65];
    urlp* rlp = *rlp_p;
    const urlp* seek;
    if ((seek = urlp_at(rlp, 3))) {
        // Get version
        hs->version_remote = urlp_as_u64(seek);
    }
    if ((seek = urlp_at(rlp, 2)) && urlp_size(seek) == sizeof(h256)) {
        // Read remote nonce
        memcpy(hs->nonce_remote.b, urlp_ref(seek, NULL), sizeof(h256));
    }
    if ((seek = urlp_at(rlp, 1)) &&
        urlp_size(seek) == sizeof(uecc_public_key)) {
        // Get secret from remote public key
        buffer[0] = 0x04;
        memcpy(&buffer[1], urlp_ref(seek, NULL), urlp_size(seek));
        uecc_btoq(buffer, 65, &hs->skey_remote);
        uecc_agree(hs->skey, &hs->skey_remote);
    }
    if ((seek = urlp_at(rlp, 0)) &&
        // Get remote ephemeral public key from signature
        urlp_size(seek) == sizeof(uecc_signature)) {
        uecc_shared_secret x;
        XOR32_SET(x.b, (&hs->skey->z.b[1]), hs->nonce_remote.b);
        err = uecc_recover_bin(urlp_ref(seek, NULL), &x, &hs->ekey_remote);
    }
    // urlp_free(&rlp);
    return err;
}

int
rlpx_handshake_auth_recv(rlpx_handshake* hs,
                         const uint8_t* b,
                         size_t l,
                         urlp** rlp_p)
{
    hs->cipher_remote_len = rlpx_decrypt(hs->skey, b, l, rlp_p);
    if (hs->cipher_remote_len) {
        memcpy(hs->cipher_remote, b, hs->cipher_remote_len);
        return 0;
    } else {
        return rlpx_handshake_auth_recv_legacy(hs, b, l, rlp_p);
    }
}

int
rlpx_handshake_auth_recv_legacy(rlpx_handshake* hs,
                                const uint8_t* auth,
                                size_t l,
                                urlp** rlp_p)
{
    int err = -1;
    uint8_t b[194];
    uint64_t v = 4;
    if (!(l == 307)) return err;
    if (!(uecies_decrypt(hs->skey, NULL, 0, auth, l, b) == 194)) return err;
    if (!(*rlp_p = urlp_list())) return err;
    urlp_push(*rlp_p, urlp_item_u8(b, 65));                // signature
    urlp_push(*rlp_p, urlp_item_u8(&b[65 + 32], 64));      // pubkey
    urlp_push(*rlp_p, urlp_item_u8(&b[65 + 32 + 64], 32)); // nonce
    urlp_push(*rlp_p, urlp_item_u64(&v, 1));               // version
    return 0;
}

int
rlpx_handshake_ack_install(rlpx_handshake* hs, urlp** rlp_p)
{
    int err = -1;
    uint8_t buff[65];
    const urlp* seek;
    urlp* rlp = *rlp_p;
    if ((seek = urlp_at(rlp, 0)) &&
        (urlp_size(seek) == sizeof(uecc_public_key))) {
        buff[0] = 0x04;
        memcpy(&buff[1], urlp_ref(seek, NULL), urlp_size(seek));
        uecc_btoq(buff, 65, &hs->ekey_remote);
    }
    if ((seek = urlp_at(rlp, 1)) && (urlp_size(seek) == sizeof(h256))) {
        memcpy(hs->nonce_remote.b, urlp_ref(seek, NULL), sizeof(h256));
        err = 0;
    }
    if ((seek = urlp_at(rlp, 2))) hs->version_remote = urlp_as_u64(seek);
    return err;
}

int
rlpx_handshake_ack_recv(rlpx_handshake* hs,
                        const uint8_t* ack,
                        size_t l,
                        urlp** rlp_p)
{
    hs->cipher_remote_len = rlpx_decrypt(hs->skey, ack, l, rlp_p);
    if (hs->cipher_remote_len) {
        memcpy(hs->cipher_remote, ack, hs->cipher_remote_len);
        return 0;
    } else {
        return rlpx_handshake_ack_recv_legacy(hs, ack, l, rlp_p);
    }
}

int
rlpx_handshake_ack_recv_legacy(rlpx_handshake* hs,
                               const uint8_t* ack,
                               size_t l,
                               urlp** rlp_p)
{
    int err = -1;
    uint8_t b[194];
    uint64_t v = 4;
    if (!(uecies_decrypt(hs->skey, NULL, 0, ack, l, b) > 0)) return err;
    if (!(*rlp_p = urlp_list())) return err;
    urlp_push(*rlp_p, urlp_item_u8(b, 64));      // pubkey
    urlp_push(*rlp_p, urlp_item_u8(&b[64], 32)); // nonce
    urlp_push(*rlp_p, urlp_item_u64(&v, 1));     // ver
    return 0;
}

//
//
//
