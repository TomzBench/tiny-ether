/*
 * @file rlpx.c
 *
 * @brief rlpx wire protocol
 */

#include "mtm/urlp.h"
#include "board_net.h"
#include "rlpx.h"

typedef struct
{
    board_socket_fd conn; /*!< os socket handle */ //
} rlpx_conn;

rlpx_conn*
rlpx_connect()
{
    rlpx_conn* c = rlpx_malloc_fn(sizeof(rlpx_conn));
    if (!c) return c; // early bail
    memset(c, 0, sizeof(rlpx_conn));
    return c;
}

void
rlpx_close(rlpx_conn** conn_p)
{
    rlpx_conn* c = *conn_p;
    *conn_p = NULL;
    rlpx_free_fn(c);
}
