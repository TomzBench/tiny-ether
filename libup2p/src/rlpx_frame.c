#include "rlpx_frame.h"

// TODO frame-egress
int frame_ingress(rlpx* s,
                  const uint8_t* x,
                  const uint8_t* expect,
                  const uint8_t* cipher,
                  size_t cipher_len,
                  uint8_t* out);
int
rlpx_frame_parse(rlpx* s, const uint8_t* frame, size_t l)
{
    int err = -1;
    uint32_t sz = 0;
    uint8_t temp[32];
    if (l < 32) return -1;

    err = frame_ingress(s, frame, &frame[16], frame, 16, temp);
    if (err) return err;

    // Read in big endian
    READ_BE(3, &sz, temp);

    // Check length (accounts for aes padding)
    if (l < (32 + (AES_LEN(sz)) + 16)) return -1;

    // Parse body
    frame += 32;
    l -= 32;
    uint8_t body[AES_LEN(sz)];
    ukeccak256_update(&s->imac, (uint8_t*)frame, AES_LEN(sz));
    ukeccak256_digest(&s->imac, temp);
    err = frame_ingress(s, temp, frame + AES_LEN(sz), frame, AES_LEN(sz), body);
    return err;
}

int
frame_ingress(rlpx* s,
              const uint8_t* x,
              const uint8_t* expect,
              const uint8_t* cipher,
              size_t cipher_len,
              uint8_t* out)
{
    // Check mac and decrypt
    // HEADER:
    // ingress.update(aes(mac-secret,ingress-mac) ^ header-ciphertext).digest
    //
    // FRAME: (ingress frame ciphertext before calling))
    // ingres-mac.update(aes(mac-secret,ingres-mac) ^
    // left128(ingres-mac.update(frame-ciphertext).digest))
    uint8_t tmp[32];
    ukeccak256_update(&s->imac, NULL, 0);
    ukeccak256_digest(&s->imac, tmp);          // mac-secret
    uaes_crypt_ecb_enc(&s->aes_mac, tmp, tmp); // aes_mac(secret)
    XORN(tmp, x, 16);                          // aes_mac(secret)^header-cipher
    ukeccak256_update(&s->imac, tmp, 16);      // ingress(...)
    ukeccak256_digest(&s->imac, tmp);          // ingress(...).digest
    if (memcmp(tmp, expect, 16)) return -1;    // compare expect with actual
    if (uaes_crypt_ctr_update(&s->aes_dec, cipher, cipher_len, out)) return -1;
    return 0;
}

//
//
//
