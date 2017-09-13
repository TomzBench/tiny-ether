/*
 * @file rlpx.c
 *
 * @brief
 */

#include "mtm/urlp.h"

#include "board_net.h"
#include "mtm/ecc.h"
#include "mtm/ecies.h"
#include "rlpx_config.h"

typedef struct
{
    board_socket_fd conn;               /*!< os socket handle */
    ucrypto_ecc_ctx ekey;               /*!< our epheremal key */
    ucrypto_ecc_ctx skey;               /*!< our static key */
    ucrypto_ecc_public_key ekey_remote; /*!< their ephermeral pubkey */
    uint64_t version_remote;
} rlpx;

rlpx* rlpx_alloc();
rlpx* rlpx_alloc_key(const char*);
rlpx* rlpx_alloc_keypair(const char*, const char*);
void rlpx_free(rlpx** session_p);

rlpx*
rlpx_alloc()
{
    return rlpx_alloc_keypair(NULL, NULL);
}

rlpx*
rlpx_alloc_key(const char* s)
{
    return rlpx_alloc_keypair(s, NULL);
}

rlpx*
rlpx_alloc_keypair(const char* s, const char* e)
{
    rlpx* session = rlpx_malloc_fn(sizeof(rlpx));
    if (session) {
        if (s) {
            ucrypto_ecc_key_init_string(&session->skey, 16, s);
        } else {
            ucrypto_ecc_key_init_new(&session->skey);
        }
        if (e) {
            ucrypto_ecc_key_init_string(&session->ekey, 16, e);
        } else {
            ucrypto_ecc_key_init_new(&session->ekey);
        }
    }
    return session;
}

void
rlpx_free(rlpx** session_p)
{
    rlpx* s = *session_p;
    *session_p = NULL;
    ucrypto_ecc_key_deinit(&s->skey);
    ucrypto_ecc_key_deinit(&s->ekey);
    rlpx_free_fn(s);
}

uint64_t
rlpx_version_remote(rlpx* s)
{
    return s->version_remote;
}

int
rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (auth[0] << 8 | auth[1]) : *(uint16_t*)auth;
    uint8_t plain[sz];
    int err = 0;
    urlp *rlp, *seek = NULL;
    l = ucrypto_ecies_decrypt(&s->skey, auth, 2, &auth[2], l - 2, plain);
    if (l > 0) {
        rlp = urlp_parse(plain, l);
        // if((seek=urlp_at(0))) //read signature
        // if((seek=urlp_at(1))) //read pubkey
        // if((seek=urlp_at(2))) //read nonce
        // if((seek=urlp_at(3))) //read ver
        if ((seek = urlp_at(rlp, 0))) {
        }
        if ((seek = urlp_at(rlp, 1))) {
        }
        if ((seek = urlp_at(rlp, 2))) {
        }
        if ((seek = urlp_at(rlp, 3))) {
            s->version_remote = urlp_as_u64(seek);
        }
        urlp_free(&rlp);
    }
    return err;
}

// cpp-ethereum reads [sig,key,nonce,ver]
//_remotePubk.ref().copyTo(m_remote.ref());
//_remoteNonce.ref().copyTo(m_remoteNonce.ref());
// m_remoteVersion = _remoteVersion;
// Secret sharedSecret;
// crypto::ecdh::agree(m_host->m_alias.secret(), _remotePubk, sharedSecret);
// m_ecdheRemote = recover(_sig, sharedSecret.makeInsecure() ^ _remoteNonce);

//
//
//
