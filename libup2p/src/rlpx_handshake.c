/**
 * @file rlpx_handshake.c
 *
 * @brief rlpx_handshake
 */

#include "rlpx_handshake.h"
#include "mtm/ecies.h"
#include "mtm/urlp.h"
#include <string.h>

void
rlpx_handshake_init(rlpx_handshake* handshake)
{
    memset(handshake, 0, sizeof(rlpx_handshake));
}

void
rlpx_handshake_deinit(rlpx_handshake** handshake)
{
    ((void)handshake);
}

int
rlpx_read_auth(rlpx_handshake* hs,
               ucrypto_ecc_ctx* skey,
               uint8_t* auth,
               size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (auth[0] << 8 | auth[1]) : *(uint16_t*)auth;
    uint8_t plain[sz];
    int err = 0;
    urlp *rlp, *seek = NULL;
    l = ucrypto_ecies_decrypt(skey, auth, 2, &auth[2], l - 2, plain);
    if (l > 0) {
        rlp = urlp_parse(plain, l);
        // if((seek=urlp_at(0))) //read signature
        // if((seek=urlp_at(1))) //read pubkey
        // if((seek=urlp_at(2))) //read nonce
        // if((seek=urlp_at(3))) //read ver
        if ((seek = urlp_at(rlp, 3))) hs->remote_version = urlp_ref_u64(seek);
        urlp_free(&rlp);
    }
    return err;
}

//
//
//
