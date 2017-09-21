/*
 * @file rlpx.c
 *
 * @brief
 */

#include "rlpx_handshake.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "urlp.h"

int
rlpx_read_auth(rlpx* s, uint8_t* auth, size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (auth[0] << 8 | auth[1]) : *(uint16_t*)auth;
    uint8_t buffer[sz]; /*assert sz >65 we reuse buffer*/
    int err = -1;
    urlp *rlp, *seek = NULL;
    l = uecies_decrypt(&s->skey, auth, 2, &auth[2], l - 2, buffer);
    if ((l > 0) && (rlp = urlp_parse(buffer, l))) {
        // if((seek=urlp_at(3))) //read ver
        // if((seek=urlp_at(2))) //read nonce
        // if((seek=urlp_at(1))) //read pubkey
        // if((seek=urlp_at(0))) //read signature
        if ((seek = urlp_at(rlp, 3))) {
            // Get version
            s->remote_version = urlp_as_u64(seek);
        }
        if ((seek = urlp_at(rlp, 2)) && urlp_size(seek) == sizeof(h256)) {
            // Read remote nonce
            memcpy(s->remote_nonce.b, urlp_ref(seek, NULL), sizeof(h256));
        }
        if ((seek = urlp_at(rlp, 1)) &&
            urlp_size(seek) == sizeof(uecc_public_key)) {
            // Get secret from remote public key
            buffer[0] = 0x04;
            memcpy(&buffer[1], urlp_ref(seek, NULL), urlp_size(seek));
            uecc_btoq(buffer, 65, &s->remote_skey);
            uecc_agree(&s->skey, &s->remote_skey);
        }
        if ((seek = urlp_at(rlp, 0)) &&
            // Get remote ephemeral public key from signature
            urlp_size(seek) == sizeof(uecc_signature)) {
            uecc_shared_secret x;
            for (int i = 0; i < 32; i++) {
                x.b[i] = s->skey.z.b[i + 1] ^ s->remote_nonce.b[i];
            }
            err = uecc_recover_bin(urlp_ref(seek, NULL), &x, &s->remote_ekey);
        }
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_read_ack(rlpx* s, uint8_t* ack, size_t l)
{
    static int x = 1;
    uint16_t sz = *(uint8_t*)&x ? (ack[0] << 8 | ack[1]) : *(uint16_t*)ack;
    uint8_t buffer[sz]; /*assert sz >65 we reuse buffer*/
    int err = -1;
    urlp *rlp, *seek = NULL;
    l = uecies_decrypt(&s->skey, ack, 2, &ack[2], l - 2, buffer);
    if ((l > 0) && (rlp = urlp_parse(buffer, l))) {
        if ((seek = urlp_at(rlp, 0)) &&
            (urlp_size(seek) == sizeof(uecc_public_key))) {
            buffer[0] = 0x04;
            memcpy(&buffer[1], urlp_ref(seek, NULL), urlp_size(seek));
            uecc_btoq(buffer, 65, &s->remote_ekey);
        }
        if ((seek = urlp_at(rlp, 1)) && (urlp_size(seek) == sizeof(h256))) {
            memcpy(s->remote_nonce.b, urlp_ref(seek, NULL), sizeof(h256));
        }
        if ((seek = urlp_at(rlp, 2))) s->remote_version = urlp_as_u64(seek);
        urlp_free(&rlp);
        err = 0;
    }
    return err;
}

//
//
//
