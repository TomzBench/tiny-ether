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

// Mock overrides
int test_mock_ready(usys_socket_fd*);
int test_mock_connect(usys_socket_fd* fd, const char* host, int port);
void test_mock_close(usys_socket_fd* fd);
int
test_mock_send(usys_socket_fd* fd, const byte* b, uint32_t l, usys_sockaddr*);
int test_mock_recv(usys_socket_fd* fd, byte* b, uint32_t l, usys_sockaddr*);

async_io_settings g_io_mock_settings = { //
    .on_connect = rlpx_io_on_connect,
    .on_accept = rlpx_io_on_accept,
    .on_erro = rlpx_io_on_erro,
    .on_send = rlpx_io_on_send,
    .connect = test_mock_connect,
    .ready = test_mock_ready,
    .close = test_mock_close,
    .tx = test_mock_send,
    .rx = test_mock_recv
};
int
test_mock_connect(usys_socket_fd* fd, const char* host, int port)
{
    ((void)host);
    ((void)port);
    static int sock = 0;     // 0 is a valid socket.
    if (*fd >= 0) return -1; // err already connect
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
    *fd = -1;
}

int
test_mock_send(
    usys_socket_fd* fd,
    const byte* b,
    uint32_t l,
    usys_sockaddr* addr)
{
    ((void)fd);
    ((void)b);
    ((void)addr);
    return l; // Sent all...
}

int
test_mock_recv(usys_socket_fd* fd, byte* b, uint32_t l, usys_sockaddr* addr)
{
    ((void)fd);
    ((void)b);
    ((void)l);
    ((void)addr);
    return 0;
}
