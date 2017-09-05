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

const ecp_point*
ecdh_pubkey(ecdh_ctx* ctx)
{
    return &ctx->Q;
}

const mpi*
ecdh_secret(ecdh_ctx* ctx)
{
    return &ctx->z;
}

int
ecdh_agree(ecdh_ctx* ctx, const ecp_point* qp)
{

    int err;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    err = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(err == 0)) goto EXIT;

    // Create shared secret with other guys Q
    err = mbedtls_ecdh_compute_shared(&ctx->grp, &ctx->z, qp, &ctx->d,
                                      mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;
EXIT:
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return err == 0 ? err : -1;
}

void
ecp_signature_init(ecp_signature* sig)
{
    mpi_init(&sig->r);
    mpi_init(&sig->s);
}

void
ecp_signature_free(ecp_signature* sig)
{
    mpi_free(&sig->r);
    mpi_free(&sig->s);
}

int
ecdh_sign(ecdh_ctx* ctx, const uint8_t* b, uint32_t sz, ecp_signature* sig)
{
    int err;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    err = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_ecdsa_sign(&ctx->grp, &sig->r, &sig->s, &ctx->d, b, sz,
                             mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;

EXIT:
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return err;
}

int
ecdh_verify(const ecp_point* q,
            const uint8_t* b,
            uint32_t sz,
            ecp_signature* sig)
{
    int err, ret = -1;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(err == 0)) goto EXIT;
    err = mbedtls_ecdsa_verify(&grp, b, sz, q, &sig->r, &sig->s);
    if (!(err == 0)) goto EXIT;
    ret = 0;
EXIT:
    mbedtls_ecp_group_free(&grp);
    return ret;
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
