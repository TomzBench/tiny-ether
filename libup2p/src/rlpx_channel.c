#include "rlpx_channel.h"
#include "rlpx_handshake.h"
#include "rlpx_helper_macros.h"
#include "unonce.h"

rlpx_channel*
rlpx_ch_alloc()
{
    return rlpx_ch_alloc_keypair(NULL, NULL);
}

rlpx_channel*
rlpx_ch_alloc_key(uecc_private_key* s)
{
    return rlpx_ch_alloc_keypair(s, NULL);
}

rlpx_channel*
rlpx_ch_alloc_keypair(uecc_private_key* s, uecc_private_key* e)
{
    rlpx_channel* session = rlpx_malloc_fn(sizeof(rlpx_channel));
    if (session) {
        // clean mem
        memset(session, 0, sizeof(rlpx_channel));

        // update info
        session->listen_port = 44;         // TODO
        memset(session->node_id, 'A', 65); // TODO

        // Create keys
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
rlpx_ch_free(rlpx_channel** session_p)
{
    rlpx_channel* s = *session_p;
    *session_p = NULL;
    uecc_key_deinit(&s->skey);
    uecc_key_deinit(&s->ekey);
    rlpx_free_fn(s);
}

uint64_t
rlpx_ch_version_remote(rlpx_channel* s)
{
    return s->remote_version;
}

const uecc_public_key*
rlpx_ch_pub_skey(rlpx_channel* s)
{
    return &s->skey.Q;
}

const uecc_public_key*
rlpx_ch_pub_ekey(rlpx_channel* s)
{
    return &s->ekey.Q;
}

const uecc_public_key*
rlpx_ch_remote_pub_ekey(rlpx_channel* s)
{
    return &s->remote_ekey;
}

const uecc_public_key*
rlpx_ch_remote_pub_skey(rlpx_channel* s)
{
    return &s->remote_skey;
}

uint32_t
rlpx_ch_listen_port(rlpx_channel* s)
{
    return s->listen_port;
}

const char*
rlpx_ch_node_id(rlpx_channel* s)
{
    return s->node_id;
}

int
rlpx_ch_auth_write(rlpx_channel* ch,
                   const uecc_public_key* to,
                   uint8_t* auth,
                   size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_auth_write(&ch->skey, &ch->ekey, &ch->nonce, to, auth, l);
}

int
rlpx_ch_auth_load(rlpx_channel* ch, const uint8_t* auth, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt authentication packet
    if ((err = rlpx_auth_read(ch, auth, l, &rlp))) return err;

    // Process the Decrypted RLP data
    err = rlpx_auth_load(&ch->skey, &ch->remote_version, &ch->remote_nonce,
                         &ch->remote_skey, &ch->remote_ekey, &rlp);

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_ch_ack_write(rlpx_channel* ch,
                  const uecc_public_key* to,
                  uint8_t* ack,
                  size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_ack_write(&ch->skey, &ch->ekey, &ch->nonce, to, ack, l);
}

int
rlpx_ch_ack_load(rlpx_channel* ch, const uint8_t* ack, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt acknowledge packet
    if ((err = rlpx_ack_read(ch, ack, l, &rlp))) return err;

    // Process the Decrypted RLP data
    err = rlpx_ack_load(&ch->remote_version, &ch->remote_nonce,
                        &ch->remote_ekey, &rlp);

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

//
//
//
