#include "uecc.h"
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
    for (int i = 0; i < 32; i++) ctx->d.b[i] = rand();
    ctx->grp = secp256k1_context_create(SECP256K1_CONTEXT_SIGN |
                                        SECP256K1_CONTEXT_VERIFY);
    if (!secp256k1_ec_pubkey_create(ctx->grp, &ctx->Q, ctx->d.b)) {
        return -1;
    }
    return 0;
}

int
uecc_key_init_binary(uecc_ctx* ctx, const uecc_private_key* d)
{
    return 0;
}

void
uecc_key_deinit(uecc_ctx* ctx)
{
}

int
uecc_atop(const char* str, int rdx, uecc_public_key* q)
{
    return 0;
}

// int
// uecc_ptob(uecc_public_key* p, uecc_public_key* b)
//{
//    return 0;
//}

// int
// uecc_btop(uecc_public_key* k, uecc_public_key* p)
//{
//    return 0;
//}

int
uecc_point_copy(const uecc_public_key* src, uecc_public_key* dst)
{
    return 0;
}

int
uecc_point_cmp(const uecc_public_key* a, const uecc_public_key* b)
{
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    return 0;
}

int
uecc_sign(uecc_ctx* ctx, const byte* b, size_t sz, uecc_signature* sig_p)
{
    return 0;
}

int
uecc_verify(const uecc_public_key* q,
            const byte* b,
            size_t sz,
            uecc_signature* sig_p)
{
    return 0;
}
int
uecc_recover(const uecc_signature* sig,
             const byte* digest,
             int recid,
             uecc_public_key* key)
{
    return 0;
}

#define FROMHEX_MAXLEN 512
const byte*
fromhex(const char* str)
{
    static byte buf[FROMHEX_MAXLEN];
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
