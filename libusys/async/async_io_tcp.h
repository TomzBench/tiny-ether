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

#ifndef ASYNC_ASYNC_IO_TCP_H_
#define ASYNC_ASYNC_IO_TCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "async_io.h"

typedef int (*async_io_tcp_on_connect_fn)(void*);
typedef int (*async_io_tcp_on_accept_fn)(void*);
typedef int (*async_io_tcp_on_erro_fn)(void*);
typedef int (*async_io_tcp_on_send_fn)(void*, int, const uint8_t*, uint32_t);
typedef int (*async_io_tcp_on_recv_fn)(void*, int err, uint8_t* b, uint32_t);

typedef struct async_io_tcp_settings
{
    async_io_tcp_on_connect_fn on_connect;
    async_io_tcp_on_accept_fn on_accept;
    async_io_tcp_on_erro_fn on_erro;
    async_io_tcp_on_send_fn on_send;
    async_io_tcp_on_recv_fn on_recv;
} async_io_tcp_settings;

typedef struct async_io_tcp_mock_settings
{
    usys_io_send_fn tx;
    usys_io_recv_fn rx;
    usys_io_ready_fn ready;
    usys_io_connect_fn connect;
    usys_io_close_fn close;
} async_io_tcp_mock_settings;

typedef struct async_io_tcp
{
    async_io base;
    async_io_tcp_on_connect_fn on_connect;
    async_io_tcp_on_accept_fn on_accept;
    async_io_tcp_on_erro_fn on_erro;
    async_io_tcp_on_send_fn on_send;
    async_io_tcp_on_recv_fn on_recv;
    usys_io_send_fn tx;
    usys_io_recv_fn rx;
    usys_io_ready_fn ready;
    usys_io_connect_fn connect;
    usys_io_close_fn close;
} async_io_tcp;

void async_io_tcp_init(async_io_tcp* io, async_io_tcp_settings* s, void* ctx);
void async_io_tcp_deinit(async_io_tcp* io);
void async_io_tcp_install_mock(
    async_io_tcp* tcp,
    async_io_tcp_mock_settings* mock);

int async_io_tcp_connect(async_io_tcp* tcp, const char* ip, uint32_t p);
int async_io_tcp_accept(async_io_tcp* tcp);
int async_io_tcp_send(async_io_tcp* tcp);
int async_io_tcp_recv(async_io_tcp* tcp);

int async_io_tcp_poll_connect(async_io* io);
int async_io_tcp_poll_send(async_io* io);
int async_io_tcp_poll_recv(async_io* io);

static inline void
async_io_tcp_state_erro_set(async_io_tcp* tcp)
{
    async_io_state_erro_set(&tcp->base);
    tcp->base.poll = async_io_tcp_poll_connect;
}

static inline void
async_io_tcp_state_ready_set(async_io_tcp* tcp)
{
    async_io_state_ready_set(&tcp->base);
    async_io_state_recv_set(&tcp->base);
    tcp->base.poll = async_io_tcp_poll_recv;
}

static inline void
async_io_tcp_state_recv_set(async_io_tcp* tcp)
{
    async_io_state_recv_set(&tcp->base);
    tcp->base.poll = async_io_tcp_poll_recv;
}

static inline void
async_io_tcp_state_send_set(async_io_tcp* tcp)
{
    async_io_state_send_set(&tcp->base);
    tcp->base.poll = async_io_tcp_poll_send;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
