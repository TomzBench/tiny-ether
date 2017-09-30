#include "rlpx_frame.h"

int
rlpx_frame_parse(ukeccak256_ctx* ingress,
                 uaes_ctx* mac,
                 const uint8_t* frame,
                 size_t l)
{
    int err;
    uint8_t tmp[32];
    ukeccak256_digest(ingress, tmp);
    uaes_crypt_ecb_enc(mac, tmp, tmp);
    XORN(tmp, frame, 16);
    ukeccak256_update(ingress, tmp, 16);
    ukeccak256_digest(ingress, tmp);
    err = memcmp(tmp, &frame[16], 16) ? -1 : 0;
    return err;
}
