#include "dh.h"

int
ucrypto_ecdh_key_init(ucrypto_ecdh_ctx* ctx, const ucrypto_mpi* d)
{
    return d ? ucrypto_ecdh_import_keypair(ctx, d)
             : ucrypto_ecdh_init_keypair(ctx);
}

int
ucrypto_ecdh_init_keypair(ucrypto_ecdh_ctx* ctx)
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

int
ucrypto_ecdh_import_keypair(ucrypto_ecdh_ctx* ctx, const ucrypto_mpi* d)
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

    // Copy our key
    ret = mbedtls_mpi_copy(&ctx->d, d);
    if (!(ret == 0)) goto EXIT;

    // Get public key from private key?
    ret = mbedtls_ecp_mul(&ctx->grp, &ctx->Q, &ctx->d, &ctx->grp.G,
                          mbedtls_entropy_func, &entropy);
    if (!(ret == 0)) goto EXIT;

EXIT:
    if (ret) mbedtls_ecdh_free(ctx);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

void
ucrypto_ecdh_key_deinit(ucrypto_ecdh_ctx* ctx)
{
    mbedtls_ecdh_free(ctx);
}

const ucrypto_ecp_point*
ucrypto_ecdh_pubkey(ucrypto_ecdh_ctx* ctx)
{
    return &ctx->Q;
}

int
ucrypto_ecdh_pubkey_write(ucrypto_ecdh_ctx* ctx, ucrypto_ecdh_public_key* b)
{
    int err;
    size_t len = 65;
    err = mbedtls_ecp_point_write_binary(
        &ctx->grp, &ctx->Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &len, *b, 65);
    return err ? -1 : 0;
}

const ucrypto_mpi*
ucrypto_ecdh_secret(ucrypto_ecdh_ctx* ctx)
{
    return &ctx->z;
}

int
ucrypto_ecdh_agree(ucrypto_ecdh_ctx* ctx, const ucrypto_ecdh_public_key* key)
{
    int err;
    ucrypto_ecp_point point;
    mbedtls_ecp_point_init(&point);

    /**
     * @brief note mbedtls_ecp_point_read_binary only accepts types of points
     * where key[0] is 0x04...
     */
    mbedtls_ecp_point_read_binary(&ctx->grp, &point, (uint8_t*)key,
                                  sizeof(ucrypto_ecdh_public_key));

    err = ucrypto_ecdh_agree_point(ctx, &point);
    mbedtls_ecp_point_free(&point);
    return err;
}

int
ucrypto_ecdh_agree_point(ucrypto_ecdh_ctx* ctx, const ucrypto_ecp_point* qp)
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
ucrypto_ecdh_sign(ucrypto_ecdh_ctx* ctx,
                  const uint8_t* b,
                  uint32_t sz,
                  ucrypto_ecp_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = *sig_p;
    ucrypto_mpi r, s;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    for (int i = 0; i < 65; i++) sig[0] = 0;

    // Init stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    ucrypto_mpi_init(&r);
    ucrypto_mpi_init(&s);

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

    // TODO: ``\_("/)_/``
    // No idea if this is right. (Lowest bit of public keys Y coordinate)
    sig[64] = mbedtls_mpi_get_bit(&ctx->Q.Y, 0) ? 1 : 0;

EXIT:
    ucrypto_mpi_free(&r);
    ucrypto_mpi_free(&s);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int
ucrypto_ecdh_verify(const ucrypto_ecp_point* q,
                    const uint8_t* b,
                    uint32_t sz,
                    ucrypto_ecp_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = *sig_p;
    mbedtls_ecp_group grp;
    ucrypto_mpi r, s;

    // Init stack content
    mbedtls_ecp_group_init(&grp);
    ucrypto_mpi_init(&r);
    ucrypto_mpi_init(&s);
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
    ucrypto_mpi_free(&r);
    ucrypto_mpi_free(&s);
    return ret;
}

//
//
//
