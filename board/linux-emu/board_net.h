#ifndef BOARD_NET_H_
#define BOARD_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

// net types.
typedef unsigned char uchar;
typedef unsigned char byte;
typedef int board_socket_fd;
typedef uint32_t board_ipaddr;
typedef unsigned char board_macaddr[6];

#ifndef hexnum
#define hexnum(c)                                                              \
    ((c <= '9') ? (c - '0') : ((c <= 'F') ? (c - 'A' + 10) : (c - 'a' + 10)))
#endif

int board_listen(int port);
int board_connect(const char*, int port);
int board_accept(void*);
int board_accept_fd(board_socket_fd s);
int board_recv(void*, uchar*, size_t);
int board_recv_fd(board_socket_fd, unsigned char* buff, size_t len);
int board_send(void*, const uchar*, size_t);
int board_send_fd(board_socket_fd, const unsigned char*, size_t len);
void board_close(void*);
void board_close_fd(board_socket_fd);
int board_select(int* sock, int nsock, int time);
int board_aton(const char* str, board_ipaddr* ipdata);
int board_atomac(const char* str, board_macaddr mac);
int board_init(const char* mac);
int board_dhcp();
int board_dhcp_wait(char*, char*, char*);
int board_static(const char*, const char*, const char*);
int board_get_ip(char* ipstr, char* snstr, char* gwstr);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
