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

#include "async_io_udp.h"

void
async_io_udp_init(async_io_udp* io, async_io_udp_settings* s, void* ctx)
{
    async_io_init(&io->base, ctx);
    io->tx = usys_send_to;
    io->rx = usys_recv_from;
    io->on_erro = s->on_erro;
    io->on_recv = s->on_recv;
    io->on_send = s->on_send;
    io->base.poll = async_io_udp_poll_recv;
}

void
async_io_udp_deinit(async_io_udp* io)
{
    async_io_deinit(&io->base);
}

void
async_io_udp_install_mock(async_io_udp* udp, async_io_udp_mock_settings* mock)
{
    udp->tx = mock->tx;
    udp->rx = mock->rx;
    udp->base.close = mock->close;
}

int
async_io_udp_send(async_io_udp* udp, uint32_t ip, uint32_t port)
{
    async_io* io = (async_io*)udp;
    if ((async_io_has_sock(io)) && (!async_io_state_send(io))) {
        udp->addr.ip = ip;
        udp->addr.port = port;
        async_io_state_send_set(io);
        io->poll = async_io_udp_poll_send;
        return 0;
    } else {
        return -1;
    }
}

int
async_io_udp_poll_send(async_io* io)
{
}

int
async_io_udp_poll_recv(async_io* io)
{
}
