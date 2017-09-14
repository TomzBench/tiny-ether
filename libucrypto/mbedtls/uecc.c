#include "uecc.h"

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format on

int
uecc_key_init(uecc_ctx* ctx, const ubn* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    int err = -1;
    ubn d;
    ubn_init(&d);
    err = ubn_str(&d, radix, s);
    if (!(err == 0)) goto EXIT;
    err = uecc_key_init_binary(ctx, &d);
    if (!(err == 0)) goto EXIT;
    err = 0;
EXIT:
    ubn_free(&d);
    return err;
}

int
uecc_key_init_binary(uecc_ctx* ctx, const ubn* d)
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

int
uecc_key_init_new(uecc_ctx* ctx)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rng;

    // initialize stack variables and callers ecc context.
    mbedtls_ecdh_init(ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&rng);

    // Seed rng
    ret = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(ret == 0)) goto EXIT;

    // Load curve parameters
    ret = mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(ret == 0)) goto EXIT;

    // Create ecc public/private key pair
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
uecc_key_deinit(uecc_ctx* ctx)
{
    mbedtls_ecdh_free(ctx);
}

int
uecc_atop(const char* str, int rdx, uecc_point* q)
{
    int err = -1;
    uint8_t buff[65];
    size_t l;
    mbedtls_ecp_group grp;
    mbedtls_mpi bin;

    // init stack
    mbedtls_mpi_init(&bin);
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(err == 0)) goto EXIT;

    // Read in string
    mbedtls_mpi_read_string(&bin, rdx, str);
    l = mbedtls_mpi_size(&bin);
    if (!(l == 65)) goto EXIT;
    mbedtls_mpi_write_binary(&bin, buff, l);
    err = mbedtls_ecp_point_read_binary(&grp, q, buff, l);
    if (!(err == 0)) goto EXIT;

    err = 0;

EXIT:
    // Free
    mbedtls_mpi_free(&bin);
    mbedtls_ecp_group_free(&grp);
    return err;
}

int
uecc_ptob(uecc_point* p, uecc_public_key* b)
{
    int err;
    size_t len = 65;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!err) {
        err = mbedtls_ecp_point_write_binary(
            &grp, p, MBEDTLS_ECP_PF_UNCOMPRESSED, &len, b->b, 65);
    }
    mbedtls_ecp_group_free(&grp);
    return err ? -1 : 0;
}

int
uecc_btop(uecc_public_key* k, uecc_point* p)
{
    int err = -1;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_point_read_binary(&grp, p, k->b, 65);
    mbedtls_ecp_group_free(&grp);
    return err;
}

int
uecc_point_copy(const uecc_point* src, uecc_point* dst)
{
    return mbedtls_ecp_copy(dst, src) == 0 ? 0 : -1;
}

int
uecc_point_cmp(const uecc_point* src, const uecc_point* dst)
{
    return mbedtls_ecp_point_cmp(src, dst);
}

int
uecc_ztoa(const uecc_point* p)
{
    return -1;
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    int err;
    uecc_point point;
    mbedtls_ecp_point_init(&point);

    /**
     * @brief note mbedtls_ecp_point_read_binary only accepts types of points
     * where key[0] is 0x04...
     */
    mbedtls_ecp_point_read_binary(&ctx->grp, &point, (uint8_t*)key,
                                  sizeof(uecc_public_key));

    err = uecc_agree_point(ctx, &point);
    mbedtls_ecp_point_free(&point);
    return err;
}

int
uecc_agree_point(uecc_ctx* ctx, const uecc_point* qp)
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
uecc_sign(uecc_ctx* ctx, const uint8_t* b, uint32_t sz, uecc_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = sig_p->b;
    ubn r, s;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    for (int i = 0; i < 65; i++) sig[0] = 0;

    // Init stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    ubn_init(&r);
    ubn_init(&s);

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
    ubn_free(&r);
    ubn_free(&s);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int
uecc_verify(const uecc_point* q,
            const uint8_t* b,
            uint32_t sz,
            uecc_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = sig_p->b;
    mbedtls_ecp_group grp;
    ubn r, s;

    // Init stack content
    mbedtls_ecp_group_init(&grp);
    ubn_init(&r);
    ubn_init(&s);
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
    ubn_free(&r);
    ubn_free(&s);
    return ret;
}
int
uecc_recover(const uecc_signature* sig,
             const uint8_t* digest,
             int recid,
             uecc_public_key* key)
{
    int err = 0;
    ubn r, s, e;
    uecc_point cp, cp2;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&e);
    mbedtls_ecp_point_init(&cp);
    mbedtls_ecp_point_init(&cp2);

    // external/trezor-crypto/ecdsa.c
    IF_ERR_EXIT(mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1));
    IF_ERR_EXIT(mbedtls_mpi_read_binary(&r, sig->b, 32));
    IF_ERR_EXIT(mbedtls_mpi_read_binary(&s, &sig->b[32], 32));
    IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&r, &grp.N));
    IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&s, &grp.N));

    // cp = R = k * G (k is secret nonce when signing)
    mbedtls_mpi_copy(&cp.X, &r);
    if (recid & 2) {
        mbedtls_mpi_add_mpi(&cp.X, &cp.X, &grp.N);
        IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&cp.X, &grp.N));
    }

// TODO -

// compute y from x
// uncompress_coords(curve, recid & 1, &cp.x, &cp.y);
// y is x
// y is x^2
// y is x^2+a
// y is x^3+ax
// Y is x^3+ax+b
// y = sqrt(y)
// e = -digest
// r := r^-1
// cp := s * R = s * k *G
// cp2 := -digest * G
// cp := (s * k - digest) * G = (r*priv) * G = r * Pub
// cp := r^{-1} * r * Pub = Pub

EXIT:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&e);
    mbedtls_ecp_point_free(&cp);
    mbedtls_ecp_point_free(&cp2);
    return err;
}

//
//
//
