#include "rlpx_internal.h"

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

const uecc_public_key*
rlpx_remote_public_skey(rlpx* s)
{
    return &s->remote_skey;
}

void
rlpx_test_nonce(rlpx* s, h256* nonce)
{
    memcpy(s->nonce.b, nonce->b, 32);
}

void
rlpx_remote_test_nonce(rlpx* s, h256* nonce)
{
    memcpy(s->remote_nonce.b, nonce->b, 32);
}
//
//
//
