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

typedef struct async_io
{
    usys_socket_fd sock;
    uint32_t state, c, len;
    uint8_t b[1200];
    void* ctx;
    usys_io_close_fn close;
    int (*poll)(struct async_io*);
} async_io;

void async_io_init(async_io* io, void* ctx);
void async_io_deinit(async_io* io);

int async_io_poll_n(async_io** io, uint32_t n, uint32_t ms);

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
async_io_memcpy(async_io* self, uint32_t idx, void* mem, size_t l)
{
    self->len = idx + l;
    return memcpy(&self->b[idx], mem, l);
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
    return ASYNC_IO_IS_READY(io->state) ? ASYNC_IO_IS_SEND(io->state) : 0;
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
