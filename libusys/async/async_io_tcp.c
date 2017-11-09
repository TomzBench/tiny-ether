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

#include "async_io_tcp.h"

void
async_io_tcp_init(async_io_tcp* io, async_io_tcp_settings* s, void* ctx)
{
    async_io_init(&io->base, ctx);
    io->connect = usys_connect;
    io->ready = usys_sock_ready;
    io->tx = usys_send;
    io->rx = usys_recv;
    io->on_erro = s->on_erro;
    io->on_connect = s->on_connect;
    io->on_accept = s->on_accept;
    io->on_send = s->on_send;
    io->on_recv = s->on_recv;
    io->base.poll = async_io_tcp_poll_connect;
}

void
async_io_tcp_deinit(async_io_tcp* tcp)
{
    async_io_deinit(&tcp->base);
}

void
async_io_tcp_install_mock(async_io_tcp* tcp, async_io_tcp_mock_settings* mock)
{
    tcp->tx = mock->tx;
    tcp->rx = mock->rx;
    tcp->ready = mock->ready;
    tcp->connect = mock->connect;
    tcp->base.close = mock->close;
}

int
async_io_tcp_connect(async_io_tcp* tcp, const char* ip, uint32_t p)
{
    async_io* io = (async_io*)tcp;
    int ret;
    if (async_io_has_sock(io)) async_io_close(io);
    ret = tcp->connect(&io->sock, ip, p);
    if (ret < 0) {
        async_io_tcp_state_erro_set(tcp);
    } else if (ret == 0) {
        async_io_tcp_state_send_set(tcp);
    } else {
        async_io_tcp_state_ready_set(tcp);
        tcp->on_connect(io->ctx);
    }
    return ret;
}

int
async_io_tcp_accept(async_io_tcp* tcp)
{
    // TODO - stub
    async_io* io = (async_io*)tcp;
    if (async_io_has_sock(io)) async_io_close(io);
    async_io_tcp_state_recv_set(tcp);
    return 0;
}

int
async_io_tcp_send(async_io_tcp* tcp)
{
    if ((!(ASYNC_IO_IS_SEND(tcp->base.state))) &&
        async_io_has_sock(&tcp->base)) {
        // If we are already not in send state and have a socket
        async_io_tcp_state_send_set(tcp);
        return 0;
    } else {
        // We are busy sending already or not connected
        return -1;
    }
}

int
async_io_tcp_recv(async_io_tcp* tcp)
{
    if (async_io_has_sock(&tcp->base)) {
        async_io_tcp_state_recv_set(tcp);
        return 0;
    } else {
        return -1;
    }
}

int
async_io_tcp_poll_connect(async_io* io)
{
    int ret = 0;
    async_io_tcp* tcp = (async_io_tcp*)io;
    if (async_io_has_sock(&tcp->base)) {
        ret = tcp->ready(&tcp->base.sock);
        if (ret < 0) {
            async_io_tcp_state_erro_set(tcp);
        } else {
            async_io_tcp_state_ready_set(tcp);
            ret = tcp->on_connect(io->ctx);
        }
    } else {
    }
    return ret;
}

int
async_io_tcp_poll_send(async_io* io)
{
    int ret = -1, c = 0, end = io->len;
    async_io_tcp* tcp = (async_io_tcp*)io;
    for (c = 0; c < 2; c++) {
        ret = tcp->tx(&io->sock, &io->b[io->c], io->len - io->c);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                tcp->on_send(io->ctx, 0, io->b, io->len);
                async_io_tcp_state_recv_set(tcp);
                ret = 0;
                break;
            } else if (ret == 0) {
                ret = 0; // OK, but maybe more to send
                break;
            } else {
                io->c += ret;
                ret = 0; // OK, but maybe more to send
            }
        } else {
            tcp->on_erro(io->ctx); // IO error
            async_io_tcp_state_erro_set(tcp);
            break;
        }
    }
    return ret;
}

int
async_io_tcp_poll_recv(async_io* io)
{
    int ret = -1, end = io->len;
    async_io_tcp* tcp = (async_io_tcp*)io;

    for (int c = 0; c < 2; c++) {
        ret = tcp->rx(&io->sock, &io->b[io->c], io->len - io->c);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                // Buffer isn't big enough
                tcp->on_erro(io->ctx);
                async_io_tcp_state_erro_set(tcp);
                break;
            } else if (ret == 0) {
                if (c == 0) {
                    // When a readable socket returns 0 bytes on first then
                    // that means remote has disconnected.
                    async_io_tcp_state_erro_set(tcp);
                } else {
                    // Looks like we read every thing.
                    tcp->on_recv(io->ctx, 0, io->b, io->c);
                    ret = 0; // OK no more data
                }
                break;
            } else {
                // Read in some data maybe try and read more (no break)
                io->c += ret;
                ret = 0;
            }
        } else {
            // rx io erro
            tcp->on_erro(io->ctx); // IO error
            async_io_tcp_state_erro_set(tcp);
        }
    }
    return ret;
}
