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
    const ucrypto_ecdh_ctx** static_key_ref;
    ucrypto_ecdh_ctx ephemeral_key;
} rlpx_session;

rlpx_session*
rlpx_session_alloc(const ucrypto_ecdh_ctx** key_p)
{
    rlpx_session* session = rlpx_malloc_fn(sizeof(rlpx_session));
    if (session) {
        ucrypto_ecdh_key_init(&session->ephemeral_key, NULL);
        session->static_key_ref = key_p;
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
rlpx_session_read_auth(rlpx_session* session, uint8_t* b, uint32_t sz)
{
}

//
//
//
