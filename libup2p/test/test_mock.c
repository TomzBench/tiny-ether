#include "test.h"

// Mock overrides
int test_mock_ready(usys_socket_fd*);
int test_mock_connect(usys_socket_fd* fd, const char* host, int port);
void test_mock_close(usys_socket_fd* fd);
int test_mock_send(usys_socket_fd* fd, const byte* b, uint32_t l);
int test_mock_recv(usys_socket_fd* fd, byte* b, uint32_t l);

async_io_settings g_io_mock_settings = { //
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
    return 0;
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
test_mock_send(usys_socket_fd* fd, const byte* b, uint32_t l)
{
    ((void)fd);
    ((void)b);
    return l; // Sent all...
}

int
test_mock_recv(usys_socket_fd* fd, byte* b, uint32_t l)
{
    ((void)fd);
    ((void)b);
    ((void)l);
    return 0;
}
