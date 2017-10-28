#ifndef RLPX_DISCOVERY_H_
#define RLPX_DISCOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_config.h"
#include "uecc.h"
#include "urlp.h"
#include "usys_io.h"

int rlpx_disc_parse(usys_sockaddr* addr,
                    const uint8_t* b,
                    uint32_t l,
                    uecc_public_key* node_id,
                    int* type,
                    urlp** rlp);

#ifdef __cplusplus
}
#endif
#endif
