#ifndef RLPX_DISCOVERY_H_
#define RLPX_DISCOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uecc.h"
#include "urlp.h"
#include "usys_io.h"

typedef enum {
    RLPX_DISCOVERY_PING = 1,
    RLPX_DISCOVERY_PONG = 2,
    RLPX_DISCOVERY_FIND = 3,
    RLPX_DISCOVERY_NEIGHBOURS = 4
} RLPX_DISCOVERY;

int rlpx_discovery_recv(usys_sockaddr* addr, const uint8_t* b, uint32_t l);
int rlpx_discovery_parse(usys_sockaddr* addr,
                         const uint8_t* b,
                         uint32_t l,
                         uecc_public_key* node_id,
                         int* type,
                         urlp** rlp);

int rlpx_discovery_parse_ping(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_parse_pong(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_parse_find(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_parse_neighbours(usys_sockaddr* addr, const urlp** rlp);

#ifdef __cplusplus
}
#endif
#endif
