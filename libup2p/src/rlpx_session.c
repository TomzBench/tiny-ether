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
    ucrypto_ecc_ctx ephemeral_key;
} rlpx_session;

rlpx_session*
rlpx_session_alloc()
{
    rlpx_session* session = rlpx_malloc_fn(sizeof(rlpx_session));
    if (session) {
        ucrypto_ecc_key_init_new(&session->ephemeral_key);
    }
    return session;
}

void
rlpx_session_free(rlpx_session** session_p)
{
    rlpx_session* session = *session_p;
    *session_p = NULL;
    ucrypto_ecc_key_deinit(&session->ephemeral_key);
    rlpx_free_fn(session);
}

int
rlpx_session_read_auth(rlpx_session* session,
                       ucrypto_ecc_ctx* secret,
                       uint8_t* cipher,
                       size_t len,
                       uint8_t* plain)
{
    // ecies decrypt with static_key_ref private key.
    int err;
    err = ucrypto_ecies_decrypt(secret, cipher, len, plain);

    return err;
}

//
//
//
