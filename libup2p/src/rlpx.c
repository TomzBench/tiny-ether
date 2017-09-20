/*
 * @file rlpx.c
 *
 * @brief
 */

#include "urlp.h"

//#include "board_net.h"
#include "rlpx_config.h"
#include "uecc.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"

typedef struct
{
    // board_socket_fd conn;        /*!< os socket handle */
    uecc_ctx ekey;               /*!< our epheremal key */
    uecc_ctx skey;               /*!< our static key */
    h512 remote_node_id;         /*!< remote public address */
    h256 remote_nonce;           /*!< remote nonce */
    uecc_public_key remote_ekey; /*!< remote ephermeral pubkey */
    uecc_public_key remote_skey; /*!< remote static pubkey */
    uint64_t remote_version;     /*!< remote version from auth */
} rlpx;

rlpx* rlpx_alloc();
rlpx* rlpx_alloc_key(uecc_private_key*);
rlpx* rlpx_alloc_keypair(uecc_private_key*, uecc_private_key*);
void rlpx_free(rlpx** session_p);

rlpx*
rlpx_alloc()
{
    return rlpx_alloc_keypair(NULL, NULL);
}

rlpx*
rlpx_alloc_key(uecc_private_key* s)
{
    return rlpx_alloc_keypair(s, NULL);
}

rlpx*
rlpx_alloc_keypair(uecc_private_key* s, uecc_private_key* e)
{
    rlpx* session = rlpx_malloc_fn(sizeof(rlpx));
    if (session) {
        if (s) {
            uecc_key_init_binary(&session->skey, s);
        } else {
            uecc_key_init_new(&session->skey);
        }
        if (e) {
            uecc_key_init_binary(&session->ekey, e);
        } else {
            uecc_key_init_new(&session->ekey);
        }
    }
    return session;
}

void
rlpx_free(rlpx** session_p)
{
    rlpx* s = *session_p;
    *session_p = NULL;
    uecc_key_deinit(&s->skey);
    uecc_key_deinit(&s->ekey);
    rlpx_free_fn(s);
}

uint64_t
rlpx_version_remote(rlpx* s)
{
    return s->remote_version;
}

const uecc_public_key*
rlpx_public_skey(rlpx* s)
{
    return &s->skey.Q;
}

const uecc_public_key*
rlpx_public_ekey(rlpx* s)
{
    return &s->ekey.Q;
}

const uecc_public_key*
rlpx_remote_public_ekey(rlpx* s)
{
    return &s->remote_ekey;
}

int
rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (auth[0] << 8 | auth[1]) : *(uint16_t*)auth;
    uint8_t buffer[sz]; /*assert sz >65 we reuse buffer*/
    int err = 0;
    urlp *rlp, *seek = NULL;
    l = uecies_decrypt(&s->skey, auth, 2, &auth[2], l - 2, buffer);
    if (l > 0) {
        rlp = urlp_parse(buffer, l);
        // if((seek=urlp_at(3))) //read ver
        // if((seek=urlp_at(2))) //read nonce
        // if((seek=urlp_at(1))) //read pubkey
        // if((seek=urlp_at(0))) //read signature
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
            // uecc_agree_bin(&s->skey, urlp_ref(seek, NULL), urlp_size(seek));
        }
        if ((seek = urlp_at(rlp, 0)) &&
            // Get remote ephemeral public key from signature
            urlp_size(seek) == sizeof(uecc_signature)) {
            uecc_shared_secret x;
            for (int i = 0; i < 32; i++) {
                x.b[i] = s->skey.z.b[i + 1] ^ s->remote_nonce.b[i];
            }
            uecc_recover_bin(urlp_ref(seek, NULL), &x, &s->remote_ekey);
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
