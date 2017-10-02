#include "rlpx_frame.h"

int frame_ingress(ukeccak256_ctx* mac,
                  uaes_ctx* aes_mac,
                  uaes_ctx* aes_dec,
                  const uint8_t* x,
                  const uint8_t* expect,
                  const uint8_t* cipher,
                  size_t cipher_len,
                  uint8_t* out);

int
rlpx_frame_parse(rlpx* s, const uint8_t* frame, size_t l)
{
    int err = -1;
    urlp *total = NULL, *header = NULL, *body = NULL;
    static uint32_t be = 1, sz = 0;
    uint8_t tmp[32];
    if (l < 32) return -1;

    // Check mac and decrypt
    // ingress.update(aes(mac-secret,ingress-mac) ^ header-ciphertext).digest
    // ukeccak256_update(&s->imac, NULL, 0);
    // ukeccak256_digest(&s->imac, tmp);          // mac-secret
    // uaes_crypt_ecb_enc(&s->aes_mac, tmp, tmp); // aes_mac(mac-secret)
    // XORN(tmp, frame, 16);                 //
    // aes_mac(mac-secret)^header-cipher
    // ukeccak256_update(&s->imac, tmp, 16); // ingress(...)
    // ukeccak256_digest(&s->imac, tmp);     // ingress(...).digest
    // if (memcmp(tmp, &frame[16], 16)) return -1; // compare expect with actual
    // if (uaes_crypt_ctr_update(&s->aes_dec, frame, 16, tmp)) return -1;
    err = frame_ingress(&s->imac, &s->aes_mac, &s->aes_dec, frame, &frame[16],
                        frame, 16, tmp);
    if (err) return err;

    // Read in big endian
    if (*(char*)&be == 0) { /*!< if we are big endian, read into mem.*/
        for (int i = 0; i < 3; i++) ((uint8_t*)&sz)[i] = tmp[i];
    } else {
        for (int i = 0; i < 3; i++) ((uint8_t*)&sz)[2 - i] = tmp[i];
    }

    // Check length (accounts for aes padding)
    if (l < (32 + (sz % 16 ? (sz + 16 - (sz % 16)) : sz) + 16)) return -1;

    // Parse body
    err = rlpx_frame_parse_body(
        s, frame + 32, frame + 32 + ((sz % 16) ? (sz + 16 - sz % 16) : sz),
        l - 32);
    return err;
}

int
rlpx_frame_parse_body(rlpx* s,
                      const uint8_t* frame,
                      const uint8_t* frame_mac,
                      size_t l)
{
    // ingres-mac.update(aes(mac-secret,ingres-mac) ^
    // left128(ingres-mac.update(frame-ciphertext).digest))
    size_t sz = frame_mac - frame;
    uint8_t tmp[32], macseed[32], body[sz];
    ukeccak256_update(&s->imac, (uint8_t*)frame, sz);
    ukeccak256_digest(&s->imac, macseed);
    ukeccak256_update(&s->imac, NULL, 0);
    ukeccak256_digest(&s->imac, tmp);          // mac-secret
    uaes_crypt_ecb_enc(&s->aes_mac, tmp, tmp); // aes_mac(mac-secret)
    XORN(tmp, macseed, 16);               // aes_mac(mac-secret)^(mac(frame))
    ukeccak256_update(&s->imac, tmp, 16); // ingress(...)
    ukeccak256_digest(&s->imac, tmp);     // ingress(...).digest
    if (memcmp(frame_mac, tmp, 16)) return -1;
    if (uaes_crypt_ctr_update(&s->aes_dec, frame, sz, body)) return -1;
    return 0;
}

int
frame_ingress(ukeccak256_ctx* mac,
              uaes_ctx* aes_mac,
              uaes_ctx* aes_dec,
              const uint8_t* x,
              const uint8_t* expect,
              const uint8_t* cipher,
              size_t cipher_len,
              uint8_t* out)
{
    uint8_t tmp[32];
    ukeccak256_update(mac, NULL, 0);
    ukeccak256_digest(mac, tmp);            // mac-secret
    uaes_crypt_ecb_enc(aes_mac, tmp, tmp);  // aes_mac(secret)
    XORN(tmp, x, 16);                       // aes_mac(secret)^header-cipher
    ukeccak256_update(mac, tmp, 16);        // ingress(...)
    ukeccak256_digest(mac, tmp);            // ingress(...).digest
    if (memcmp(tmp, expect, 16)) return -1; // compare expect with actual
    if (uaes_crypt_ctr_update(aes_dec, cipher, cipher_len, out)) return -1;
    return 0;
}
//
//
//
