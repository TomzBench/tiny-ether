#include "dh.h"
#include "board_mem.h"

crypto_ecdh_ctx*
crypto_ecdh_key_alloc()
{
    int err;
    crypto_ecdh_ctx* ctx = board_alloc(sizeof(crypto_ecdh_ctx));
    if (!ctx) return ctx;
    err = crypto_ecdh_key_init(ctx);
    if (!(err == 0)) crypto_ecdh_key_free(&ctx);
    return ctx;
}

int
crypto_ecdh_key_init(crypto_ecdh_ctx* ctx)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rng;

    // initialize stack variables and callers ecdh context.
    mbedtls_ecdh_init(ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&rng);
    ret = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(ret == 0)) goto EXIT;

    ret = mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(ret == 0)) goto EXIT;

    ret = mbedtls_ecdh_gen_public(&ctx->grp, &ctx->d, &ctx->Q,
                                  mbedtls_ctr_drbg_random, &rng);
    if (!(ret == 0)) goto EXIT;

EXIT:
    if (ret) mbedtls_ecdh_free(ctx);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

void
crypto_ecdh_key_free(crypto_ecdh_ctx** ctx_p)
{
    crypto_ecdh_ctx* ctx = *ctx_p;
    *ctx_p = NULL;
    mbedtls_ecdh_free(ctx);
}
