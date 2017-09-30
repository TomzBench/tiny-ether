#include "rlpx_frame.h"

// def ceil16(x):
//    return x if x % 16 == 0 else x + 16 - (x % 16)

int
rlpx_frame_parse(ukeccak256_ctx* ingress,
                 uaes_ctx* mac,
                 const uint8_t* frame,
                 size_t l)
{
    if (l < 32) return -1;
    urlp* header = rlpx_frame_parse_header(ingress, mac, frame, 32);
    if (!header) return -1;
}

urlp*
rlpx_frame_parse_header(ukeccak256_ctx* ingress,
                        uaes_ctx* mac,
                        const uint8_t* frame,
                        size_t l)
{
    urlp* rlp = NULL;
    uint8_t tmp[32];
    ukeccak256_digest(ingress, tmp);
    uaes_crypt_ecb_enc(mac, tmp, tmp);
    XORN(tmp, frame, 16);
    ukeccak256_update(ingress, tmp, 16);
    ukeccak256_digest(ingress, tmp);
    if (memcmp(tmp, &frame[16], 16)) return rlp;
}

urlp*
rlpx_frame_parse_data(ukeccak256_ctx* ingress,
                      uaes_ctx* mac,
                      const uint8_t* frame,
                      size_t l)
{
}
