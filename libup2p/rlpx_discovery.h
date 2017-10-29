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

typedef struct
{
    uint8_t ip[16]; /*!< BE encoding ipv4 or ipv3 data */
    uint32_t tcp;   /*!< devp2p port */
    uint32_t udp;   /*!< p2p port */
} rlpx_discovery_endpoint;

typedef struct
{
    uecc_public_key nodeid;     /*!< pubkey */
    rlpx_discovery_endpoint ep; /*!< remote endpoint routing*/
    urlp* meta;                 /*!< meta data (topics?) */
} rlpx_discovery_node;

int rlpx_discovery_recv(usys_sockaddr* addr, const uint8_t* b, uint32_t l);
int rlpx_discovery_parse(usys_sockaddr* addr,
                         const uint8_t* b,
                         uint32_t l,
                         uecc_public_key* node_id,
                         int* type,
                         urlp** rlp);

int rlpx_discovery_parse_ping(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_ping(uint32_t ver,
                              uint32_t timestamp,
                              const rlpx_discovery_endpoint* ep_src,
                              const rlpx_discovery_endpoint* ep_dst,
                              uint8_t* dst,
                              uint32_t* l);
int rlpx_discovery_parse_pong(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_pong(uint32_t timestamp,
                              h256* echo,
                              const rlpx_discovery_endpoint* ep_to,
                              uint8_t* d,
                              uint32_t* l);
int rlpx_discovery_parse_find(usys_sockaddr* addr, const urlp** rlp);
int rlpx_discovery_print_find(uint8_t* nodeid,
                              uint32_t timestamp,
                              uint8_t* b,
                              uint32_t* l);
int rlpx_discovery_parse_neighbours(usys_sockaddr* addr, const urlp** rlp);
//  rlpx_discovery_print_neighbours( ....TODO

#ifdef __cplusplus
}
#endif
#endif
