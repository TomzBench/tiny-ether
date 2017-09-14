#include "uecc.h"
#include <string.h>

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

static void uecc_create_key(bignum256* k, const bignum256* prime);
extern ecdsa_curve secp256k1;

int
uecc_key_init(uecc_ctx* ctx, const ubn* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    int err = 0;
    ubn d;
    IF_ERR_EXIT(ubn_str(&d, radix, s));
    IF_ERR_EXIT(uecc_key_init_binary(ctx, &d));
EXIT:
    return err;
}

int
uecc_key_init_new(uecc_ctx* ctx)
{
    memset(ctx, 0, sizeof(uecc_ctx));
    ctx->grp = &secp256k1;
    uecc_create_key(&ctx->d, &ctx->grp->order);
    scalar_multiply(ctx->grp, &ctx->d, &ctx->Q);
    return 0;
}

int
uecc_key_init_binary(uecc_ctx* ctx, const ubn* d)
{
    int err = 0;
    memset(ctx, 0, sizeof(uecc_ctx));
    bn_copy(d, &ctx->d);
    ctx->grp = &secp256k1;
    scalar_multiply(ctx->grp, &ctx->d, &ctx->Q);
    return err;
}

void
uecc_key_deinit(uecc_ctx* ctx)
{
}

int
uecc_atop(const char* str, int rdx, uecp_point* q)
{
    return 0;
}

int
uecc_ptob(uecp_point* p, uecc_public_key* b)
{
    return 0;
}

int
uecc_btop(uecc_public_key* k, uecp_point* p)
{
    return 0;
}

int
uecc_point_copy(const uecp_point* src, uecp_point* dst)
{
    point_copy(src, dst);
    return 0;
}

int
uecc_point_cmp(const uecp_point* a, const uecp_point* b)
{
    return !(bn_is_equal(&a->x, &b->x) && bn_is_equal(&a->y, &b->y));
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    int err;
    curve_point p;
    IF_ERR_EXIT(ecdsa_read_pubkey(ctx->grp, key->b, &p));
    point_copy(&p, &ctx->Qp);
    IF_ERR_EXIT(uecc_agree_point(ctx, &p));
EXIT:
    return err;
}

int
uecc_agree_point(uecc_ctx* ctx, const uecp_point* qp)
{

    point_multiply(ctx->grp, &ctx->d, qp, &ctx->z);
    return 0;
}

int
uecc_sign(uecc_ctx* ctx, const uint8_t* b, uint32_t sz, uecc_signature* sig_p)
{
    return 0;
}

int
uecc_verify(const uecp_point* q,
            const uint8_t* b,
            uint32_t sz,
            uecc_signature* sig_p)
{
    return 0;
}
int
uecc_recover(const uecc_signature* sig,
             const uint8_t* digest,
             int recid,
             uecc_public_key* key)
{
    return 0;
}

// generate random K for signing/side-channel noise
static void
uecc_create_key(bignum256* k, const bignum256* prime)
{
    do {
        int i;
        for (i = 0; i < 8; i++) {
            k->val[i] = random32() & 0x3FFFFFFF;
        }
        k->val[8] = random32() & 0xFFFF;
        // check that k is in range and not zero.
    } while (bn_is_zero(k) || !bn_is_less(k, prime));
}

//
//
//
