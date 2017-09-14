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

int
uecc_key_init(uecc_ctx* ctx, const uecc_private_key* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    return 0;
}

int
uecc_key_init_new(uecc_ctx* ctx)
{
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

//
//
//
