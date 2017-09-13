#include "aes.h"

int
uaes_init(uaes_ctx* ctx, uaes_128_ctr_key* key)
{
    return 0;
}

void
uaes_deinit(uaes_ctx** ctx_p)
{
}

int
uaes_crypt(uaes_128_ctr_key* key,
           uaes_iv* iv,
           const uint8_t* in,
           size_t inlen,
           uint8_t* out)
{
    return 0;
}
