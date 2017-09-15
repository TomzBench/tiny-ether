#include "uecc.h"
#include "uhash.h"
#include <stdlib.h> /*!< temporary for rand */
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

// private
const byte* fromhex(const char* str);

int
uecc_key_init(uecc_ctx* ctx, const uecc_private_key* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    if (!(radix == 16)) return -1;
    return uecc_key_init_binary(ctx, (uecc_private_key*)fromhex(s));
}

int
uecc_key_init_new(uecc_ctx* ctx)
{
    uecc_private_key d;
    for (int i = 0; i < 32; i++) d.b[i] = rand(); // TODO not rnd
    return uecc_key_init_binary(ctx, &d);
}

int
uecc_key_init_binary(uecc_ctx* ctx, const uecc_private_key* d)
{
    memcpy(&ctx->d, d, sizeof(uecc_private_key));
    ctx->grp = secp256k1_context_create(SECP256K1_CONTEXT_SIGN |
                                        SECP256K1_CONTEXT_VERIFY);
    if (!secp256k1_ec_pubkey_create(ctx->grp, &ctx->Q, ctx->d.b)) return -1;
    return 0;
}

void
uecc_key_deinit(uecc_ctx* ctx)
{
    secp256k1_context_destroy(ctx->grp);
}

int
uecc_atop(const char* str, int rdx, uecc_public_key* q)
{
    if (!(rdx == 16)) return -1;
    memcpy(q, fromhex(str), sizeof(uecc_public_key));
    return 0;
}

int
uecc_point_copy(const uecc_public_key* src, uecc_public_key* dst)
{
    memcpy(dst, src, sizeof(uecc_public_key));
    return 0;
}

int
uecc_z_cmp_str(const uecc_shared_secret* a, const char* b)
{
    return memcmp(a->b, fromhex(b), 32);
}

int
uecc_z_cmp(const uecc_shared_secret* a, const uecc_shared_secret* b)
{
    return memcmp(a->b, b->b, 32);
}

int
uecc_point_cmp(const uecc_public_key* a, const uecc_public_key* b)
{
    return memcmp(a, b, sizeof(uecc_public_key));
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    int ok = secp256k1_ecdh(ctx->grp, ctx->z.b, key, ctx->d.b);
    return !ok;
}

int
uecc_sign(uecc_ctx* ctx, const byte* msg, size_t sz, uecc_signature* sig)
{
    if (!(sz == 32)) return -1;
    int ok = secp256k1_ecdsa_sign_recoverable(ctx->grp, sig, msg, ctx->d.b,
                                              NULL, NULL);
    return !ok;
}

int
uecc_verify(const uecc_public_key* q,
            const byte* msg,
            size_t sz,
            uecc_signature* recsig)
{

    int ok;
    ((void)sz);
    uecc_ctx ctx;
    // Convert a recoverable sig to normal sig and verify
    secp256k1_ecdsa_signature rawsig;
    ctx.grp = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    secp256k1_ecdsa_recoverable_signature_convert(ctx.grp, &rawsig, recsig);
    ok = secp256k1_ecdsa_verify(ctx.grp, &rawsig, msg, q);
    secp256k1_context_destroy(ctx.grp);
    return !ok;
}

int
uecc_recover(const uecc_point* sig,
             const byte* digest,
             int recid,
             uecc_public_key* key)
{
    return 0;
}

int
uecies_encrypt_str(uecc_ctx* p,
                   const byte* s,
                   size_t slen,
                   int radix,
                   const char* plain,
                   byte* cipher)
{
    return -1;
}

int
uecies_encrypt(uecc_point* p,
               const byte* s,
               size_t slen,
               const byte* in,
               size_t inlen,
               byte* out)
{
    return 0;
}

#define FROMHEX_MAXLEN 512
const byte*
fromhex(const char* str)
{
    static byte buf[FROMHEX_MAXLEN];
    memset(buf, 0, FROMHEX_MAXLEN);
    size_t len = strlen(str) / 2;
    if (len > FROMHEX_MAXLEN) len = FROMHEX_MAXLEN;
    for (size_t i = 0; i < len; i++) {
        byte c = 0;
        if (str[i * 2] >= '0' && str[i * 2] <= '9')
            c += (str[i * 2] - '0') << 4;
        if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
            c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
        if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
            c += (str[i * 2 + 1] - '0');
        if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
            c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
        buf[i] = c;
    }
    return buf;
}

//
//
//
