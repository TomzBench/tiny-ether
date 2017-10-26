#include "usys_io.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int
usys_connect(usys_socket_fd* sock_p, const char* host, int port)
{
    // return <0 if err, 0 if INPROGRESS, >0 if connect instant (ie local host)
    int ret = 0;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    usys_socket_fd* sock = sock_p;
    if ((*sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        return -1;
    }
    inet_pton(AF_INET, host, &addr.sin_addr);
    int rc = connect(*sock, (struct sockaddr*)&addr, sizeof(addr));
    if (rc < 0) {
        if (errno == EINPROGRESS) {
            ret = 0;
        } else {
            /* ECONNREFUSED */
            if (*sock >= 0) usys_close(sock);
            ret = -1;
        }
    } else {
        ret = 1;
    }
    return ret;
}

int
usys_listen_udp(usys_socket_fd* sock_p, int port)
{
    int ret = 0;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    *sock_p = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
    if (*sock_p < 0) return -1;
    if (bind(*sock_p, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
        usys_close(sock_p);
        return -1;
    }
    return ret;
}

int
usys_recv_fd(int sockfd, byte* b, size_t len)
{
    ssize_t bytes_read = 0;
    bytes_read = recv(sockfd, (char*)b, len, 0);
    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            bytes_read = 0;
        }
    }
    return bytes_read;
}

int
usys_send_fd(usys_socket_fd sockfd, const byte* b, uint32_t len)
{
    ssize_t bytes_sent = 0;
    bytes_sent = send(sockfd, (char*)b, len, 0);
    if (bytes_sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            bytes_sent = 0;
        }
    }
    return bytes_sent;
}

void
usys_close(usys_socket_fd* ctx)
{
    usys_close_fd(*(usys_socket_fd*)ctx);
    *(usys_socket_fd*)ctx = -1;
}

void
usys_close_fd(usys_socket_fd s)
{
    close(s);
}

uint32_t
usys_select(uint32_t* rmask,
            uint32_t* wmask,
            int time,
            int* reads,
            int nreads,
            int* writes,
            int nwrites)
{
    int *sock_p, max_fd, err;
    fd_set readfds, writefds;
    struct timeval tv;

    // Init stack
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    tv.tv_sec = 0;
    tv.tv_usec = (time * 1000);

    // Get highest socket number (for select)
    sock_p = reads;
    max_fd = *sock_p;
    for (int i = 0; i < nreads; i++) {
        if (*sock_p >= 0) FD_SET(*sock_p, &readfds);
        if (*sock_p > max_fd) max_fd = *sock_p;
        sock_p++;
    }
    sock_p = writes;
    for (int i = 0; i < nwrites; i++) {
        if (*sock_p >= 0) FD_SET(*sock_p, &writefds);
        if (*sock_p > max_fd) max_fd = *sock_p;
        sock_p++;
    }
    err = select(max_fd + 1, &readfds, &writefds, NULL, &tv);
    if (err < 0) return 0;
    sock_p = reads;

    // Set mask for sockets that have activity
    for (int i = 0; i < nreads; i++) {
        if (FD_ISSET(*sock_p, &readfds)) *rmask |= (0x01 << i);
        sock_p++;
    }
    sock_p = writes;
    for (int i = 0; i < nwrites; i++) {
        if (FD_ISSET(*sock_p, &writefds)) *wmask |= (0x01 << i);
        sock_p++;
    }
    return 0;
}

int
usys_sock_error(usys_socket_fd* sock)
{
    int optval = -1;
    socklen_t optlen = sizeof(optval);
    int err = getsockopt(*sock, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    return err ? err : (optval == EINPROGRESS) ? 0 : -1;
}

int
usys_sock_ready(usys_socket_fd* sock)
{
    // If error return < 0
    // If waiting return 0
    // If ready return > 0
    int optval = -1, err;
    socklen_t optlen = sizeof(optval);
    err = getsockopt(*sock, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    return err ? err : (optval == EINPROGRESS) ? 0 : optval ? -1 : 1;
}

//
//
//
