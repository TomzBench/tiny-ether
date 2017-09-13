#include "aes.h"

int
ucrypto_aes_init(ucrypto_aes_ctx* ctx, ucrypto_aes_128_ctr_key* key)
{
    return 0;
}

void
ucrypto_aes_deinit(ucrypto_aes_ctx** ctx_p)
{
}

int
ucrypto_aes_crypt(ucrypto_aes_128_ctr_key* key,
                  ucrypto_aes_iv* iv,
                  const uint8_t* in,
                  size_t inlen,
                  uint8_t* out)
{
    return 0;
}
