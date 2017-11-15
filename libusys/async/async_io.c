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

#include "async_io.h"

void
async_io_tcp_init(async_io* io, async_io_settings* settings, void* ctx)
{
    async_io_init(io, ctx);
    io->send = usys_send;
    io->recv = usys_recv;
    io->ready = usys_sock_ready;
    io->connect = usys_connect;
    io->on_connect = settings->on_connect;
    io->on_accept = settings->on_accept;
    io->on_error = settings->on_erro;
    io->on_send = settings->on_send;
    io->on_recv = settings->on_recv;
    io->poll = async_io_tcp_poll_connect;
}

void
async_io_udp_init(async_io* io, async_io_settings* settings, void* ctx)
{
    async_io_init(io, ctx);
    io->sendto = usys_send_to;
    io->recvfrom = usys_recv_from;
    io->on_connect = settings->on_connect;
    io->on_accept = settings->on_accept;
    io->on_error = settings->on_erro;
    io->on_send = settings->on_send;
    io->on_recv = settings->on_recv;
    io->poll = async_io_udp_poll_recv;
}

void
async_io_init(async_io* io, void* ctx)
{
    io->sock = -1;
    io->addr.ip = io->addr.port = io->c = io->len = io->state = 0;
    io->ctx = ctx;
    io->close = usys_close;
    memset(io->b, 0, sizeof(io->b));
}

void
async_io_deinit(async_io* io)
{
    if (async_io_has_sock(io)) async_io_close(io);
    memset(io->b, 0, sizeof(io->b));
}

void
async_io_install_mock(async_io* io, async_io_mock_settings* mock)
{
    if (mock->sendto) {
        io->sendto = mock->sendto;
    } else if (mock->send) {
        io->send = mock->send;
    }
    if (mock->recvfrom) {
        io->recvfrom = mock->recvfrom;
    } else if (mock->recv) {
        io->recv = mock->recv;
    }
    if (mock->close) io->close = mock->close;
    if (mock->connect) io->connect = mock->connect;
    if (mock->ready) io->ready = mock->ready;
}

int
async_io_tcp_connect(async_io* io, const char* ip, uint32_t p)
{
    int ret;
    if (async_io_has_sock(io)) async_io_close(io);
    ret = io->connect(&io->sock, ip, p);
    if (ret < 0) {
        async_io_state_erro_set(io);
        io->poll = async_io_tcp_poll_connect;
    } else if (ret == 0) {
        async_io_state_send_set(io);
    } else {
        async_io_state_ready_set(io);
        async_io_state_recv_set(io);
        io->poll = async_io_tcp_poll_recv;
        io->on_connect(io->ctx);
    }
    return ret;
}

int
async_io_tcp_accept(async_io* io)
{
    // TODO - stub
    if (async_io_has_sock(io)) async_io_close(io);
    async_io_state_recv_set(io);
    io->poll = async_io_tcp_poll_recv;
    return 0;
}

int
async_io_udp_listen(async_io* io, uint32_t port)
{
    int ret = -1;
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
async_io_tcp_send(async_io* io)
{
    if ((async_io_has_sock(io)) && (!async_io_state_busy(io))) {
        // If we are already not in send state and have a socket
        async_io_state_send_set(io);
        io->poll = async_io_tcp_poll_send;
        return 0;
    } else {
        // We are busy sending already or not connected
        return -1;
    }
}

int
async_io_udp_send(async_io* io, uint32_t ip, uint32_t port)
{
    if ((async_io_has_sock(io)) && (!async_io_state_busy(io))) {
        io->addr.ip = ip;
        io->addr.port = port;
        async_io_state_send_set(io);
        io->poll = async_io_udp_poll_send;
        return 0;
    } else {
        return -1;
    }
}

int
async_io_poll_n(async_io** io, uint32_t n, uint32_t ms)
{
    uint32_t mask = 0;
    int reads[n], writes[n], err = 0;
    for (uint32_t c = 0; c < n; c++) {
        reads[c] = async_io_state_recv(io[c]) ? io[c]->sock : -1;
        writes[c] = async_io_state_send(io[c]) ? io[c]->sock : -1;
    }
    err = usys_select(&mask, &mask, ms, reads, n, writes, n);
    if (mask) {
        for (uint32_t i = 0; i < n; i++) {
            if (mask & (0x01 << i)) err |= async_io_poll(io[i]);
        }
    }
    return err;
}

int
async_io_tcp_poll_connect(async_io* io)
{
    int ret = 0;
    if (async_io_has_sock(io)) {
        ret = io->ready(&io->sock);
        if (ret < 0) {
            // Connect error
            async_io_state_erro_set(io);
            async_io_close(io);
            io->poll = async_io_tcp_poll_connect;
        } else {
            // Connection complete
            async_io_state_ready_set(io);
            async_io_state_recv_set(io);
            io->poll = async_io_tcp_poll_recv;
            ret = io->on_connect(io->ctx);
        }
    } else {
        // Invalid socket
    }
    return ret;
}

int
async_io_tcp_poll_send(async_io* io)
{
    int ret = -1, c = 0, end = io->len, sent = 0;
    for (c = 0; c < 2; c++) {
        ret = io->send(&io->sock, &io->b[io->c], io->len - io->c);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                // Send complete - put back to recv state
                sent = io->len;
                async_io_state_recv_set(io);
                io->poll = async_io_tcp_poll_recv;
                io->on_send(io->ctx, 0, io->b, sent);
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
            io->on_error(io->ctx); // IO error
            async_io_state_erro_set(io);
            io->poll = async_io_tcp_poll_connect;
            break;
        }
    }
    return ret;
}

int
async_io_tcp_poll_recv(async_io* io)
{
    int ret = -1;

    for (int c = 0; c < 2; c++) {
        ret = io->recv(&io->sock, &io->b[io->c], io->len - io->c);
        if (ret >= 0) {
            io->c += ret;
            if (io->c >= io->len) {
                // Buffer isn't big enough
                io->on_error(io->ctx);
                async_io_state_erro_set(io);
                io->poll = async_io_tcp_poll_connect;
                break;
            } else if (ret == 0) {
                if (c == 0) {
                    // When a readable socket returns 0 bytes on first then
                    // that means remote has disconnected.
                    async_io_state_erro_set(io);
                    io->on_error(io->ctx);
                    io->poll = async_io_tcp_poll_connect;
                } else {
                    // Looks like we read every thing.
                    io->on_recv(io->ctx, 0, io->b, io->c);
                    io->c = 0;
                    ret = 0; // OK no more data
                }
                break;
            } else {
                // Read in some data maybe try and read more (no break)
                ret = 0;
            }
        } else {
            // rx io erro
            io->on_error(io->ctx); // IO error
            async_io_state_erro_set(io);
            io->poll = async_io_tcp_poll_connect;
        }
    }
    return ret;
}

int
async_io_udp_poll_send(async_io* io)
{
    int c, ret = -1, end = io->len, sent = 0;
    for (c = 0; c < 2; c++) {
        ret = io->sendto(&io->sock, &io->b[io->c], io->len - io->c, &io->addr);
        if (ret >= 0) {
            if (ret + (int)io->c == end) {
                // Send complete, put into listen mode
                sent = io->len;
                async_io_state_recv_set(io);
                io->poll = async_io_udp_poll_recv;
                io->on_send(io->ctx, 0, io->b, sent);
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
            io->on_error(io->ctx); // IO error
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

    int r = -1;
    while (1) {
        r = io->recvfrom(&io->sock, &io->b[io->c], io->len - io->c, &io->addr);
        if (r > 0) {
            io->c += r;
            if (io->c >= io->len) {
                io->on_error(io->ctx); // IO error
                async_io_state_erro_set(io);
                break;
            } else {
                io->on_recv(io->ctx, 0, io->b, io->c);
                io->c = 0;
            }
        } else if (r < 0) {
            io->on_error(io->ctx); // IO error
            async_io_state_erro_set(io);
            break;
        } else {
            break; // r==0
        }
    }

    return r;
}
