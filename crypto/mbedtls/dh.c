#include "dh.h"
#include "board_mem.h"

ecdh_ctx*
ecdh_key_alloc()
{
    int err;
    ecdh_ctx* ctx = board_alloc(sizeof(ecdh_ctx));
    if (!ctx) return ctx;
    err = ecdh_key_init(ctx);
    if (!(err == 0)) ecdh_key_free(&ctx);
    return ctx;
}

int
ecdh_key_init(ecdh_ctx* ctx)
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

int
ecdh_agree(ecdh_ctx* ctx, const uint8_t* theirs, uint32_t sz)
{

    int err;
    mbedtls_ctr_drbg_context rng;
    mbedtls_ctr_drbg_init(&rng);
    // Read other guys public key into our context.
    err = mbedtls_ecdh_read_public(ctx, theirs, sz);
    if (!(err == 0)) goto EXIT;

    // Create shared secret with other guys context.
    err = mbedtls_ecdh_compute_shared(&ctx->grp, &ctx->z, &ctx->Qp, &ctx->d,
                                      mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;
EXIT:
    mbedtls_ctr_drbg_free(&rng);
    return err == 0 ? err : -1;
}

void
ecdh_key_free(ecdh_ctx** ctx_p)
{
    ecdh_ctx* ctx = *ctx_p;
    *ctx_p = NULL;
    ecdh_key_deinit(ctx);
    board_free(ctx);
}

void
ecdh_key_deinit(ecdh_ctx* ctx)
{
    mbedtls_ecdh_free(ctx);
}

//
//
//
