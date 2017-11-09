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

#ifndef ASYNC_ASYNC_IO_UDP_H_
#define ASYNC_ASYNC_IO_UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "async_io.h"

typedef int (*async_io_udp_on_erro_fn)(void*);
typedef int (*async_io_udp_on_send_fn)(void*, int, const uint8_t*, uint32_t);
typedef int (*async_io_udp_on_recv_fn)(void*, int err, uint8_t* b, uint32_t);

typedef struct async_io_udp_settings
{
    async_io_udp_on_erro_fn on_erro;
    async_io_udp_on_send_fn on_send;
    async_io_udp_on_recv_fn on_recv;
} async_io_udp_settings;

typedef struct async_io_udp_mock_settings
{
    usys_io_send_to_fn tx;
    usys_io_recv_from_fn rx;
    usys_io_close_fn close;
} async_io_udp_mock_settings;

typedef struct async_io_udp
{
    async_io base;
    async_io_udp_on_erro_fn on_erro;
    async_io_udp_on_send_fn on_send;
    async_io_udp_on_recv_fn on_recv;
    usys_sockaddr addr;
    usys_io_send_to_fn tx;
    usys_io_recv_from_fn rx;
} async_io_udp;

void async_io_udp_init(async_io_udp* io, async_io_udp_settings* s, void* ctx);
void async_io_udp_deinit(async_io_udp* io);
void async_io_udp_install_mock(
    async_io_udp* udp,
    async_io_udp_mock_settings* mock);

int async_io_udp_send(async_io_udp* udp, uint32_t ip, uint32_t port);
int async_io_udp_listen(async_io_udp* udp, uint32_t port);

int async_io_udp_poll_send(async_io* io);
int async_io_udp_poll_recv(async_io* io);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
