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
    memset(&io->addr, 0, sizeof(usys_sockaddr));
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
async_io_udp_listen(async_io_udp* udp, uint32_t port)
{
    int ret = -1;
    async_io* io = (async_io*)udp;
    if (async_io_has_sock(io)) async_io_close(io);
    ret = usys_listen_udp(&io->sock, port);
    if (!ret) {
        async_io_state_ready_set(io);
        async_io_state_recv_set(io);
        io->poll = async_io_udp_poll_recv;
    } else {
        async_io_state_erro_set(io);
    }
    return ret;
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
    int c, ret = -1, end = io->len;
    async_io_udp* udp = (async_io_udp*)io; // up cast
    for (c = 0; c < 2; c++) {
        ret = udp->tx(&io->sock, &io->b[io->c], io->len - io->c, &udp->addr);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                // Send complete, put into listen mode
                udp->on_send(io->ctx, 0, io->b, io->len);
                async_io_state_recv_set(io);
                io->poll = async_io_udp_poll_recv;
                ret = 0;
                break;
            } else if (ret == 0) {
                ret = 0;
                break;
            } else {
                io->c += ret;
                ret = 0;
            }
        } else {
            udp->on_erro(io->ctx); // IO error
            async_io_state_erro_set(io);
            io->poll = async_io_udp_poll_recv;
            break;
        }
    }
    return ret;
}

int
async_io_udp_poll_recv(async_io* io)
{
    int ret = -1;
    async_io_udp* udp = (async_io_udp*)io; // up cast

    while (1) {
        ret = udp->rx(&io->sock, &io->b[io->c], io->len - io->c, &udp->addr);
        if (ret > 0) {
            io->c += ret;
            if (io->c >= io->len) {
                udp->on_erro(io->ctx); // IO error
                async_io_state_erro_set(io);
                break;
            } else {
                udp->on_recv(io->ctx, 0, io->b, io->c);
                io->c = 0;
            }
        } else if (ret < 0) {
            udp->on_erro(io->ctx); // IO error
            async_io_state_erro_set(io);
            break;
        } else {
            break; // ret==0
        }
    }

    /*
    for (c = 0; c < 2; c++) {
        ret = udp->rx(&io->sock, &io->b[io->c], io->len - io->c, &udp->addr);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                udp->on_erro(io->ctx);
                async_io_state_erro_set(io);
                break;
            } else if (ret == 0) {
                if (c == 0) {
                    // When a readable socket returns 0 bytes on first then
                    // that means remote has disconnected.
                    async_io_state_erro_set(io);
                } else {
                    udp->on_recv(io->ctx, 0, io->b, io->c);
                    ret = 0; // OK no more data
                }
                break;
            } else {
                io->c += ret;
                ret = 0; // OK maybe more data
            }
        } else {
            udp->on_erro(io->ctx); // IO error
            async_io_state_erro_set(io);
        }
    }
    */
    return ret;
}
