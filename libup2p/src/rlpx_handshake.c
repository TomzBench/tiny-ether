/**
 * @file rlpx_handshake.c
 *
 * @brief rlpx_handshake
 */

#include "rlpx_handshake.h"
#include "mtm/ecc.h"
#include "mtm/ecies.h"
#include "mtm/urlp.h"

int
rlpx_read_auth(ucrypto_ecc_ctx* skey, uint8_t* auth, size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (auth[0] << 8 | auth[1]) : *(uint16_t*)auth;
    uint8_t plain[sz];
    int err = 0;
    urlp* rlp;
    err = ucrypto_ecies_decrypt(skey, &auth[2], l - 2, plain);
    if (!err) {
        rlp = urlp_parse(plain, 1000);
    }
    return err;
}

//
//
//
