#ifndef ASYNC_ASYNC_IO_H_
#define ASYNC_ASYNC_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"
#include "usys_io.h"

#define ASYNC_IO_STATE_READY (0x01 << 0)
#define ASYNC_IO_STATE_ERRO (0x01 << 1)
#define ASYNC_IO_STATE_SEND (0x01 << 2)
#define ASYNC_IO_STATE_RECV (0x01 << 3)

#define ASYNC_IO_READY(x) ((x) & (ASYNC_IO_STATE_READY))
#define ASYNC_IO_SEND(x) ((x) & (ASYNC_IO_STATE_SEND))
#define ASYNC_IO_RECV(x) ((x) & (ASYNC_IO_STATE_RECV))
#define ASYNC_IO_ERRO(x) ((x) & (ASYNC_IO_STATE_ERRO))
#define ASYNC_IO_SOCK(x) ((x)->sock >= 0)

#define ASYNC_IO_SET_SEND(x)                                                   \
    do {                                                                       \
        (x)->state |= ASYNC_IO_STATE_SEND;                                     \
        (x)->state &= (~(ASYNC_IO_STATE_RECV));                                \
        (x)->c = 0;                                                            \
    } while (0)

#define ASYNC_IO_SET_RECV(x)                                                   \
    do {                                                                       \
        (x)->state |= ASYNC_IO_STATE_RECV;                                     \
        (x)->state &= (~(ASYNC_IO_STATE_SEND));                                \
        (x)->c = 0;                                                            \
        (x)->end = 1200;                                                       \
    } while (0)

#define ASYNC_IO_SET_ERRO(x)                                                   \
    do {                                                                       \
        (x)->on_erro((x)->ctx, -1, 0, 0);                                      \
        (x)->state = 0;                                                        \
        (x)->c = 0;                                                            \
        (x)->end = 0;                                                          \
        if (ASYNC_IO_SOCK((x))) usys_close(&(x)->sock);                        \
    } while (0)

#define ASYNC_IO_CLOSE(x)                                                      \
    do {                                                                       \
        (x)->state = 0;                                                        \
        (x)->c = 0;                                                            \
        (x)->end = 0;                                                          \
        if (ASYNC_IO_SOCK((x))) usys_close(&(x)->sock);                        \
    } while (0)

typedef int (*async_io_cb)(void* ctx, int err, const uint8_t* b, uint32_t l);
typedef struct
{
    usys_socket_fd sock;
    uint32_t state;
    usys_io_send_fn tx;
    usys_io_recv_fn rx;
    async_io_cb on_recv;
    async_io_cb on_send;
    async_io_cb on_erro;
    void* ctx;
    uint32_t c, end;
    uint8_t b[1200];
} async_io;

void async_io_install(usys_io_send_fn s, usys_io_recv_fn r);
void async_io_init(async_io*, void*, async_io_cb, async_io_cb, async_io_cb);
void async_io_deinit(async_io* self);
int async_io_connect(async_io* async, const char* ip, uint32_t p);
void async_io_close(async_io* self);
int async_io_send(async_io*, const char*, uint32_t);
int async_io_recv(async_io* obj);
int async_io_poll_n(async_io** io, uint32_t n, uint32_t ms);
int async_io_poll(async_io*);
int async_io_sock(async_io* self);
int async_io_has_sock(async_io* self);
int async_io_state_recv(async_io* self);
int async_io_state_send(async_io* self);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
