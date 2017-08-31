#include "board_net.h"
#include <ctype.h>

int board_connect(const char *host, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    board_socket_fd sock;
    if ((sock = (int)socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	return -1;
    }
    inet_pton(AF_INET, host, &addr.sin_addr);
    int rc = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0) {
	close(sock);
	return -1;
    }
    return sock;
}

int board_listen(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;
    int opt = true;
    int rc;
    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (rc < 0) {
	close(sockfd);
	return -1;
    }

    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    rc = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0) {
	char *err = strerror(errno);
	((void)err);  // print error to console.
	close(sockfd);
	return -1;
    }

    listen(sockfd, 10000);

    return sockfd;
}

board_socket_fd board_accept(void *ctx) {
    return board_accept_fd(*(board_socket_fd *)ctx);
}

board_socket_fd board_accept_fd(board_socket_fd s) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    return accept(s, (struct sockaddr *)&addr, &addr_len);
}

int board_recv(void *ctx, uchar *b, size_t len) {
    return board_recv_fd(*(board_socket_fd *)ctx, b, len);
}

int board_recv_fd(int sockfd, unsigned char *b, size_t len) {
    int32_t active, bytes_read = 0;
    while (bytes_read < len) {
	const int32_t n =
	    recv(sockfd, (char *)b + bytes_read, len - bytes_read, 0);
	if (n == -1 && errno == EINTR) continue;
	if (n == -1) return -1;
	if (n == 0) return bytes_read;
	bytes_read += n;
	active = board_select(&sockfd, 1, 80);
	if (!(active == sockfd)) break;
    }
    return bytes_read;
}

int board_send(void *ctx, const uchar *b, size_t len) {
    return board_send_fd(*(board_socket_fd *)ctx, b, len);
}

int board_send_fd(int sockfd, const unsigned char *b, size_t len) {
    size_t bytes_sent = 0;
    while (bytes_sent < len) {
	const int32_t rc =
	    send(sockfd, (char *)b + bytes_sent, len - bytes_sent, 0);
	if (rc == -1 && errno == EINTR) continue;
	if (rc == -1) return -1;
	if (rc == 0) break;
	bytes_sent += rc;
    }
    return bytes_sent;
}

void board_close(void *ctx) {
    return board_close_fd(*(board_socket_fd *)ctx);
}

void board_close_fd(board_socket_fd s) { close(s); }

int board_select(int *sock, int nsock, int time) {
    if (!nsock) return -1;
    fd_set readfds;
    FD_ZERO(&readfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = (time * 1000);

    int *sock_p = sock;
    int max_fd = *sock_p;
    for (int i = 0; i < nsock; i++) {
	if (*sock_p >= 0) FD_SET(*sock_p, &readfds);
	if (*sock_p > max_fd) max_fd = *sock_p;
	sock_p++;
    }
    int result = select(max_fd + 1, &readfds, NULL, NULL, &tv);
    if (result < 0) return result;
    sock_p = sock;
    for (int i = 0; i < nsock; i++) {
	if (FD_ISSET(*sock_p, &readfds)) {
	    if ((!*sock_p) && (nsock == 1)) return true;
	    return *sock_p;
	}
	sock_p++;
    }
    return 0;
}

int board_aton(const char *str, board_ipaddr *ipdata) {
    uint32_t num[4] = {0};
    int i, index = 0, len = strlen(str);
    uint32_t temp = 0;

    if (ipdata == NULL) return -1;

    for (i = 0; str[i] && /*(i<16)*/ (i < len) && (index < 4); i++) {
	if (isdigit(str[i])) {
	    num[index] = num[index] * 10 + str[i] - '0';
	} else if (str[i] == '.') {
	    index++;
	} else {
	    return -1;
	}
    }

    if (/*(str[i] == '\0')*/ (i == len) && (index == 3)) {
	for (i = 0; i < 4; i++) {
	    if (num[i] > 255) {
		return -1;
	    } else {
		temp = (temp << 8) | num[i];
	    }
	}
    } else {
	if ((len == 0) || ((len == 1) && (*str == '0'))) {
	    temp = 0;
	} else {
	    return -1;
	}
    }

    *ipdata = temp;
    return 0;
}

int board_atomac(const char *str, board_macaddr mac) {
    int i, j = 0;

    for (i = 0; i < 16; i += 3) {
	if (isxdigit(str[i]) && isxdigit(str[i + 1]) &&
	    ((str[i + 2] == ':') || (str[i + 2] == '\0'))) {
	    mac[j++] = hexnum(str[i]) * 16 + hexnum(str[i + 1]);
	} else {
	    return -1;
	}
    }
    return 0;
}

int board_init(const char *mac) {
    ((void)mac);
    return 0;
}

int board_dhcp() {
    return 0;  //
}
int board_dhcp_wait(char *ip, char *sn, char *gw) {
    ((void)ip);
    ((void)sn);
    ((void)gw);
    return 0;
}
int board_static(const char *ip, const char *sn, const char *gw) {
    ((void)ip);
    ((void)sn);
    ((void)gw);
    return 0;
}

int board_get_ip(char *ipstr, char *snstr, char *gwstr) {
    ((void)ipstr);
    ((void)snstr);
    ((void)gwstr);
    return 0;
}

//
//
//
