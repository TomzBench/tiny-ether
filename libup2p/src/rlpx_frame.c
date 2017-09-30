#include "rlpx_frame.h"

int
rlpx_frame_parse(ukeccak256_ctx* ingress,
                 uaes_ctx* aes,
                 uaes_ctx* mac,
                 const uint8_t* frame,
                 size_t l)
{
    int err = -1;
    urlp *total = NULL, *header = NULL, *body = NULL;
    static uint32_t be = 1, sz = 0;
    uint8_t tmp[32];
    if (l < 32) return -1;

    // Check mac and decrypt
    // ingress.update(aes(mac-secret,ingress-mac) ^ header-ciphertext).digest
    ukeccak256_update(ingress, NULL, 0);
    ukeccak256_digest(ingress, tmp);     // mac-secret
    uaes_crypt_ecb_enc(mac, tmp, tmp);   // aes_mac(mac-secret)
    XORN(tmp, frame, 16);                // aes_mac(mac-secret)^header-cipher
    ukeccak256_update(ingress, tmp, 16); // ingress(...)
    ukeccak256_digest(ingress, tmp);     // ingress(...).digest
    if (memcmp(tmp, &frame[16], 16)) return -1; // compare expect with actual
    if (uaes_crypt_ctr_update(aes, frame, 16, tmp)) return -1;

    // Read in big endian
    if (*(char*)&be == 0) { /*!< if we are big endian, read into mem.*/
        for (int i = 0; i < 3; i++) ((uint8_t*)&sz)[i] = tmp[i];
    } else {
        for (int i = 0; i < 3; i++) ((uint8_t*)&sz)[2 - i] = tmp[i];
    }

    // Check length (accounts for aes padding)
    if (l < (32 + (sz % 16 ? (sz + 16 - (sz % 16)) : sz) + 16)) return -1;

    // Parse body
    body = rlpx_frame_parse_body(ingress,    //
                                 aes,        //
                                 mac,        //
                                 frame + 32, //
                                 frame + 32 +
                                     ((sz % 16) ? (sz + 16 - sz % 16) : sz), //
                                 l - 32);
    return err;
}

urlp*
rlpx_frame_parse_body(ukeccak256_ctx* ingress,
                      uaes_ctx* aes,
                      uaes_ctx* mac,
                      const uint8_t* frame,
                      const uint8_t* frame_mac,
                      size_t l)
{
    // ingres-mac.update(aes(mac-secret,ingres-mac) ^
    // left128(ingres-mac.update(frame-ciphertext).digest))
    uint8_t tmp[32], macseed[32];
    size_t sz = frame_mac - frame;
    ukeccak256_update(ingress, (uint8_t*)frame, sz);
    ukeccak256_digest(ingress, macseed);
    ukeccak256_update(ingress, NULL, 0);
    ukeccak256_digest(ingress, tmp);     // mac-secret
    uaes_crypt_ecb_enc(mac, tmp, tmp);   // aes_mac(mac-secret)
    XORN(tmp, macseed, 16);              // aes_mac(mac-secret)^(mac(frame))
    ukeccak256_update(ingress, tmp, 16); // ingress(...)
    ukeccak256_digest(ingress, tmp);     // ingress(...).digest
    if (memcmp(frame_mac, tmp, 16)) return NULL;
}

//
//
//
