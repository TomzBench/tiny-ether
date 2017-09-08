/*
 * @file rlpx_session.c
 *
 * @brief
 */

#include "board_net.h"

#include "rlpx_session.h"

typedef struct
{
    board_socket_fd conn; /*!< os socket handle */ //
    ucrypto_ecdh_ctx ephemeral_key;
} rlpx_session;

rlpx_session*
rlpx_session_alloc()
{
    rlpx_session* session = rlpx_malloc_fn(sizeof(rlpx_session));
    if (session) {
        ucrypto_ecdh_key_init(&session->ephemeral_key, NULL);
    }
    return session;
}

void
rlpx_session_free(rlpx_session** session_p)
{
    rlpx_session* session = *session_p;
    *session_p = NULL;
    ucrypto_ecdh_key_deinit(&session->ephemeral_key);
    rlpx_free_fn(session);
}

int
rlpx_session_read_auth(rlpx_session* session,
                       ucrypto_ecdh_ctx* secret,
                       uint8_t* cipher,
                       size_t cipher_sz,
                       uint8_t* plain,
                       size_t plain_sz)
{
    // ecies decrypt with static_key_ref private key.
    //    ucrypto_ecies_decrypt(*session->static_key_ref, cipher, cipher_sz,
    //                          plaintext, 307);

    int err = 0;
    err = ucrypto_ecies_decrypt(secret, cipher, cipher_sz, plain, plain_sz);

    return err;
}

//
//
//
