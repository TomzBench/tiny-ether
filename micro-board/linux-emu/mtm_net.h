#ifndef MTM_NET_H_
#define MTM_NET_H_

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

typedef int mtm_socket__fd; /* item pointer */

int mtm_listen(int port);
int mtm_connect(const char *, int port);
int mtm_accept(void *);
int mtm_accept_fd(mtm_socket__fd s);
int mtm_recv(void *, uchar *, size_t);
int mtm_recv_fd(mtm_socket__fd, unsigned char *buff, size_t len);
int mtm_send(void *, const uchar *, size_t);
int mtm_send_fd(mtm_socket__fd, const unsigned char *, size_t len);
void mtm_close(void *);
void mtm_close_fd(mtm_socket__fd);
int mtm_select(int *sock, int nsock, int time);
int mtm_aton(const char *str, mtm_ipaddr *ipdata);
int mtm_atomac(const char *str, mtm_macaddr mac);
int mtm_init(const char *mac);
int mtm_dhcp();
int mtm_dhcp_wait(char *, char *, char *);
int mtm_static(const char *, const char *, const char *);
int mtm_get_ip(char *ipstr, char *snstr, char *gwstr);



#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
