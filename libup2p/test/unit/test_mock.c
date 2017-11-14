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

#include "test.h"

test_mock_socket g_test_sockets[10] = { //
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};

async_io_mock_settings g_io_mock_tcp_settings = { //
    .connect = test_mock_connect,
    .ready = test_mock_ready,
    .close = test_mock_close,
    .send = test_mock_send,
    .recv = test_mock_recv,
    .sendto = NULL,
    .recvfrom = NULL
};

async_io_mock_settings g_io_mock_udp_settings = { //
    .connect = test_mock_connect,
    .ready = test_mock_ready,
    .close = test_mock_close,
    .send = NULL,
    .recv = NULL,
    .sendto = test_mock_sendto,
    .recvfrom = test_mock_recvfrom
};

int
test_mock_connect(usys_socket_fd* fd, const char* host, int port)
{
    ((void)host);
    ((void)port);
    static int sock = 0;       // 0 is a valid socket.
    if (*fd >= 0) return -1;   // err already connect
    if (sock >= 10) return -1; // out of test sockets
    if (g_test_sockets[sock].data) {
        rlpx_free(g_test_sockets[sock].data);
        g_test_sockets[sock].data = NULL;
        g_test_sockets[sock].sz = 0;
    }
    *fd = sock++;
    return 1;
}

int
test_mock_ready(usys_socket_fd* fd)
{
    return *fd; // Always ready
}

void
test_mock_close(usys_socket_fd* fd)
{
    if (g_test_sockets[*fd].data) {
        rlpx_free(g_test_sockets[*fd].data);
        g_test_sockets[*fd].data = NULL;
        g_test_sockets[*fd].sz = 0;
    }
    *fd = -1;
}

int
test_mock_send(usys_socket_fd* fd, const byte* b, uint32_t l)
{
    if (*fd < 10) {
        // TODO - realloc and prepend instead of free
        if (g_test_sockets[*fd].data) {
            rlpx_free(g_test_sockets[*fd].data);
            g_test_sockets[*fd].data = NULL;
        }
        // Read data into sim socket
        g_test_sockets[*fd].data = rlpx_malloc(l);
        if (g_test_sockets[*fd].data) {
            memcpy(g_test_sockets[*fd].data, b, l);
            g_test_sockets[*fd].sz = l;
            return l;
        }
    }
    return -1; //
}

int
test_mock_recv(usys_socket_fd* fd, byte* b, uint32_t l)
{
    if (*fd < 10) {
        if (g_test_sockets[*fd].data) {
            // TODO if buffer not big enough realloc and shrink, return amount
            // read
            if (!(l <= g_test_sockets[*fd].sz)) return -1;
            memcpy(b, g_test_sockets[*fd].data, g_test_sockets[*fd].sz);
        } else {
            return 0; // would block
        }
    }
    return -1;
}

int
test_mock_sendto(
    usys_socket_fd* fd,
    const byte* b,
    uint32_t l,
    usys_sockaddr* addr)
{
    ((void)fd);
    ((void)b);
    ((void)addr);
    return l;
}

int
test_mock_recvfrom(usys_socket_fd* fd, byte* b, uint32_t l, usys_sockaddr* addr)
{
    ((void)fd);
    ((void)b);
    ((void)addr);
    return l;
}
