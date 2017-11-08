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
async_io_tcp_init(async_io_tcp* io, void* ctx)
{
    async_io_init(&io->base, ctx);
    io->base.poll = async_io_tcp_poll_connect;
    io->base.erro = async_io_tcp_erro;
}

void
async_io_tcp_deinit(async_io_tcp* tcp)
{
    async_io_deinit(&tcp->base);
}

int
async_io_tcp_erro(async_io* io)
{
    async_io_tcp* tcp = (async_io_tcp*)io;
    tcp->base.poll = async_io_tcp_poll_connect;
    return -1; // close socket
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
            tcp->on_send(io->ctx, -1, 0, 0); // IO error
            async_io_tcp_state_erro_set(tcp);
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
                tcp->on_recv(io->ctx, -1, 0, 0);
                async_io_tcp_state_erro_set(tcp);
                break;
            } else if (ret == 0) {
                if (c == 0) {
                    // When a readable socket returns 0 bytes on first then
                    // that means remote has disconnected.
                    async_io_tcp_state_erro_set(tcp);
                } else {
                    tcp->on_recv(io->ctx, 0, io->b, io->c);
                    ret = 0; // OK no more data
                }
                break;
            } else {
                io->c += ret;
                ret = 0; // OK maybe more data
            }
        } else {
            tcp->on_recv(io->ctx, -1, 0, 0); // IO error
            async_io_tcp_state_erro_set(tcp);
        }
    }
    return ret;
}
