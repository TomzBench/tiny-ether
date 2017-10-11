#include "rlpx_devp2p.h"

int
rlpx_devp2p_write_disconnect(rlpx_coder* x,
                             RLPX_DEVP2P_DISCONNECT_REASON reason,
                             uint8_t* out,
                             size_t* l)
{
    int err = -1;
    uint32_t tmp = *l - 1;
    urlp* body = urlp_list();
    urlp_push(body, urlp_item_u16((uint16_t*)&reason, 1));
    out[0] = DEVP2P_DISCONNECT;
    err = urlp_print(body, &out[1], &tmp);
    if (!err) rlpx_frame_write(x, 0, 0, out, tmp + 1, out, l);
    urlp_free(&body);
    return err;
}

