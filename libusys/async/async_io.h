// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

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

#define ASYNC_IO_IS_READY(x) ((x) & (ASYNC_IO_STATE_READY))
#define ASYNC_IO_IS_SEND(x) ((x) & (ASYNC_IO_STATE_SEND))
#define ASYNC_IO_IS_RECV(x) ((x) & (ASYNC_IO_STATE_RECV))
#define ASYNC_IO_IS_ERRO(x) ((x) & (ASYNC_IO_STATE_ERRO))

/**
 * @brief IO callback
 */
typedef int (*async_io_on_connect_fn)(void*);
typedef int (*async_io_on_accept_fn)(void*);
typedef int (*async_io_on_erro_fn)(void*);
typedef int (*async_io_on_send_fn)(void*, int, const uint8_t*, uint32_t);
typedef int (*async_io_on_recv_fn)(void*, int err, uint8_t* b, uint32_t);

/**
 * @brief Initialize io context with callbacks
 */
typedef struct async_io_settings
{
    async_io_on_connect_fn on_connect;
    async_io_on_accept_fn on_accept;
    async_io_on_erro_fn on_erro;
    async_io_on_send_fn on_send;
    async_io_on_recv_fn on_recv;
} async_io_settings;

/**
 * @brief Override usys_io_... with mock behavior for test
 */
typedef struct async_io_mock_settings
{
    usys_io_send_fn send;
    usys_io_send_to_fn sendto;
    usys_io_recv_fn recv;
    usys_io_recv_from_fn recvfrom;
    usys_io_ready_fn ready;
    usys_io_connect_fn connect;
    usys_io_close_fn close;
} async_io_mock_settings;

/**
 * @brief Main async io context
 * Send/Recv in a union to support different function pointer types for type
 * checking.
 */
typedef struct async_io
{
    usys_socket_fd sock;
    usys_sockaddr addr;
    void* ctx;
    uint32_t state, c, len;
    int (*poll)(struct async_io*);
    usys_io_close_fn close;
    usys_io_ready_fn ready;
    usys_io_connect_fn connect;
    async_io_on_connect_fn on_connect;
    async_io_on_accept_fn on_accept;
    async_io_on_erro_fn on_error;
    async_io_on_send_fn on_send;
    async_io_on_recv_fn on_recv;
    union
    {
        usys_io_send_fn send;
        usys_io_send_to_fn sendto;
    };
    union
    {
        usys_io_recv_fn recv;
        usys_io_recv_from_fn recvfrom;
    };
    uint8_t b[1290];
} async_io;

void async_io_tcp_init(async_io* io, async_io_settings* settings, void* ctx);
void async_io_udp_init(async_io* io, async_io_settings* settings, void* ctx);
void async_io_init(async_io* io, void* ctx);
void async_io_deinit(async_io* io);

void async_io_install_mock(async_io* io, async_io_mock_settings* mock);

int async_io_tcp_connect(async_io* tcp, const char* ip, uint32_t p);
int async_io_tcp_accept(async_io* io);
int async_io_udp_listen(async_io* io, uint32_t port);

int async_io_tcp_send(async_io* io);
int async_io_udp_send(async_io* io, uint32_t ip, uint32_t port);

int async_io_poll_n(async_io** io, uint32_t n, uint32_t ms);

int async_io_tcp_poll_connect(async_io* io);
int async_io_tcp_poll_send(async_io* io);
int async_io_tcp_poll_recv(async_io* io);

int async_io_udp_poll_send(async_io* io);
int async_io_udp_poll_recv(async_io* io);

static inline int
async_io_has_sock(async_io* io)
{
    return (io->sock >= 0);
}

static inline void
async_io_close(async_io* io)
{
    io->close(&io->sock);
    io->state = io->len = io->c = 0;
}

static inline void
async_io_on_recv(async_io* io, async_io_on_recv_fn fn)
{
    io->on_recv = fn;
}

static inline void
async_io_on_send(async_io* io, async_io_on_send_fn fn)
{
    io->on_send = fn;
}

static inline uint32_t
async_io_ip_addr(async_io* io)
{
    return io->addr.ip;
}

static inline uint32_t
async_io_port(async_io* io)
{
    return io->addr.port;
}

static inline void*
async_io_mem(async_io* self, uint32_t idx)
{
    return &self->b[idx];
}

static inline void
async_io_len_set(async_io* self, uint32_t len)
{
    self->len = len;
}

static inline uint32_t
async_io_len(async_io* self)
{
    return self->len;
}

static inline const void*
async_io_memcpy(async_io* self, void* mem, size_t l)
{
    l = (l <= self->len) ? l : self->len;
    self->len = l;
    return memcpy(self->b, mem, l);
}

static inline int
async_io_print(async_io* self, uint32_t idx, const char* fmt, ...)
{
    int l;
    va_list ap;
    va_start(ap, fmt);
    l = vsnprintf((char*)&self->b[idx], sizeof(self->b) - idx, fmt, ap);
    if (l >= 0) self->len = l;
    va_end(ap);
    return l;
}

static inline uint8_t*
async_io_buffer(async_io* io)
{
    return ((async_io*)io)->b;
}

static inline uint32_t*
async_io_buffer_length_pointer(async_io* io)
{
    return &((async_io*)io)->len;
}

static inline void
async_io_len_reset(async_io* tcp)
{
    ((async_io*)tcp)->len = sizeof((async_io*)tcp)->b;
}

static inline int
async_io_poll(async_io* io)
{
    return io->poll(io);
}

static inline int
async_io_state_ready(async_io* io)
{
    return ASYNC_IO_IS_READY(io->state);
}

static inline void
async_io_state_ready_set(async_io* io)
{
    io->state |= ASYNC_IO_STATE_READY;
    io->c = io->len = 0;
}

static inline int
async_io_state_recv(async_io* io)
{
    return ASYNC_IO_IS_READY(io->state) && ASYNC_IO_IS_RECV(io->state);
}

static inline void
async_io_state_recv_set(async_io* io)
{
    io->state |= ASYNC_IO_STATE_RECV;
    io->state &= (~(ASYNC_IO_STATE_SEND));
    io->c = 0;
    io->len = sizeof(io->b);
}

static inline int
async_io_state_send(async_io* io)
{
    return (ASYNC_IO_IS_SEND(io->state));
}

static inline int
async_io_state_busy(async_io* io)
{
    return (ASYNC_IO_IS_SEND(io->state) || (io->c));
}

static inline void
async_io_state_send_set(async_io* io)
{
    io->state |= ASYNC_IO_STATE_SEND;
    io->state &= (~(ASYNC_IO_STATE_RECV));
    io->c = 0;
}

static inline int
async_io_state_erro_set(async_io* io)
{
    io->state |= ASYNC_IO_STATE_ERRO;
    io->len = io->c = 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
