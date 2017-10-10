/**
 * @file usys_io.h
 *
 * @brief Simple wrapper around OS primitives to better support non standard
 * enviorments.
 */
#ifndef USYS_IO_H_
#define USYS_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usys_config.h"

typedef int usys_socket_fd;
typedef int usys_file_fd;
typedef unsigned char byte;

// File sys call abstraction layer
usys_file_fd usys_file_open(const char* path);
int usys_file_write(usys_file_fd* fd, int offset, const char* data, uint32_t l);
int usys_file_read(usys_file_fd* fd, int offset, char* data, uint32_t l);
int usys_file_close(usys_file_fd* fd);

// Networking sys call abstraction layer
typedef int (*usys_io_send_fn)(usys_socket_fd*, const byte*, uint32_t);
typedef int (*usys_io_recv_fn)(usys_socket_fd*, byte*, uint32_t);
int usys_connect(usys_socket_fd* fd, const char* host, int port);
int usys_send_fd(usys_socket_fd fd, const byte* b, uint32_t len);
int usys_recv_fd(int sockfd, byte* b, size_t len);
void usys_close(usys_socket_fd* fd);
void usys_close_fd(usys_socket_fd s);
int usys_sock_error(usys_socket_fd* fd);
int usys_sock_ready(usys_socket_fd* fd);
uint32_t usys_select(uint32_t* rmask,
                     uint32_t* wmask,
                     int time,
                     int* reads,
                     int nreads,
                     int* writes,
                     int nwrites);
static inline int
usys_send(usys_socket_fd* fd, const byte* b, uint32_t len)
{
    return usys_send_fd(*(usys_socket_fd*)fd, b, len);
}
static inline int
usys_recv(usys_socket_fd* fd, byte* b, uint32_t len)
{
    return usys_recv_fd(*(usys_socket_fd*)fd, b, len);
}

#ifdef __cplusplus
}
#endif
#endif
