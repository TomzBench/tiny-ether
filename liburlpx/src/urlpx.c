/*
 * @file urlpx.c
 *
 * @brief urlpx wire protocol
 */

#include "mtm/urlp.h"
#include "board_net.h"
#include "urlpx.h"

typedef struct
{
    board_socket_fd conn; /*!< os socket handle */ //
} urlpx_conn;

urlpx_conn*
urlpx_connect()
{
    urlpx_conn* c = urlpx_malloc_fn(sizeof(urlpx_conn));
    if (!c) return c; // early bail
    memset(c, 0, sizeof(urlpx_conn));
    return c;
}

void
urlpx_close(urlpx_conn** conn_p)
{
    urlpx_conn* c = *conn_p;
    *conn_p = NULL;
    urlpx_free_fn(c);
}
