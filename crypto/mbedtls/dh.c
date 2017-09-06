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

    // Seed rng
    ret = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(ret == 0)) goto EXIT;

    // Load curve parameters
    ret = mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(ret == 0)) goto EXIT;

    // Create ecdh public/private key pair
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

    // initialize stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);

    // seed RNG
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

int
ecdh_sign(ecdh_ctx* ctx, const uint8_t* b, uint32_t sz, ecp_signature sig)
{
    int err, ret = -1;
    mpi r, s;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    for (int i = 0; i < 65; i++) sig[0] = 0;

    // Init stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    mpi_init(&r);
    mpi_init(&s);

    // Seed RNG
    err = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(err == 0)) goto EXIT;

    // Sign message
    err = mbedtls_ecdsa_sign(&ctx->grp, &r, &s, &ctx->d, b, sz,
                             mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;
    ret = 0;

    // Print Keys
    err = mbedtls_mpi_write_binary(&r, &sig[0], 32);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_write_binary(&s, &sig[32], 32);
    if (!(err == 0)) goto EXIT;

    // TODO: No idea if this is right. (Lowest byte of public keys Y coordinate)
    // ecdsa_sign_recoverable(...
    //   ecdsa_sig_sign(...
    //     fe_is_odd(r.y) ? 1:0
    // ecdsa_recoverable_signature_save(...
    sig[64] = ctx->Q.Y.p[0] ? 1 : 0;

EXIT:
    mpi_free(&r);
    mpi_free(&s);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int
ecdh_verify(const ecp_point* q,
            const uint8_t* b,
            uint32_t sz,
            ecp_signature sig)
{
    int err, ret = -1;
    mbedtls_ecp_group grp;
    mpi r, s;

    // Init stack content
    mbedtls_ecp_group_init(&grp);
    mpi_init(&r);
    mpi_init(&s);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_read_binary(&r, sig, 32);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_read_binary(&s, &sig[32], 32);
    if (!(err == 0)) goto EXIT;

    // Verify signature of content
    err = mbedtls_ecdsa_verify(&grp, b, sz, q, &r, &s);
    if (!(err == 0)) goto EXIT;

    ret = 0;
EXIT:
    mbedtls_ecp_group_free(&grp);
    mpi_free(&r);
    mpi_free(&s);
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
